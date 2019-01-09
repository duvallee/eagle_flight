/*
 * File: gpio_stmpe1600.c
 *
 * Written by duvallee in 2018
 *
 */
#include "main.h"
#include "gpio_stmpe1600.h"

// ============================================================================
static I2C_HandleTypeDef* g_i2c_handle                   = NULL;

// ============================================================================
//
// @brief : Definitions for STMPE1600 I2C register addresses on 8 bit
//
#define I2cExpAddr0                                      ((int) (0x43 * 2))
#define I2cExpAddr1                                      ((int) (0x42 * 2))

#define STMPE1600_CHIP_ID_LSB                            0x00
#define STMPE1600_CHIP_ID_MSB                            0x16
#define STMPE1600_VERSION_ID                             0x01

// Register MAP
#define STMPE1600_CHIP_ID_LSB_REG                        0x00
#define STMPE1600_CHIP_ID_MSB_REG                        0x01
#define STMPE1600_VERSION_ID_REG                         0x03

#define STMPE1600_GPMR                                   0x10
#define STMPE1600_GPSR                                   0x12
#define STMPE1600_GPDR                                   0x14

#if defined(RTOS_FREERTOS)
static SemaphoreHandle_t* g_i2c_semaphore                = NULL;
#endif

#if defined(GPIO_STMPE1600)
/**
 * @defgroup XNUCLEO53L0A1_7Segment 7 segment display
 *
 * macro use for human readable segment building
 * @code
 *  --s0--
 *  s    s
 *  5    1
 *  --s6--
 *  s    s
 *  4    2
 *  --s3-- . s7 (dp)
 * @endcode
 *
 * @{
 */
/** decimal point bit mapping*  */
#define DP                                               (1 << 7)
// sgement s0 bit mapping
#define S0                                               (1 << 3)
// sgement s1 bit mapping
#define S1                                               (1 << 5)
// sgement s2 bit mapping
#define S2                                               (1 << 6)
// sgement s3 bit mapping
#define S3                                               (1 << 4)
// sgement s4 bit mapping
#define S4                                               (1 << 0)
// sgement s5 bit mapping
#define S5                                               (1 << 1)
// sgement s6 bit mapping
#define S6                                               (1 << 2)

/**
 * build a character by defining the non lighted segment (not one and no DP)
 *
 * @param  ... literal sum and or combine of any macro to define any segment #S0 .. #S6
 *
 * example '9' is all segment on but S4
 * @code
 *   ['9']=           NOT_7_NO_DP(S4),
 * @endcode
 */
#define NOT_7_NO_DP(...)                                 (uint8_t) ~( __VA_ARGS__ + DP )

/**
 * Ascii to 7 segment  lookup table
 *
 * Most common character are supported and follow http://www.twyman.org.uk/Fonts/
 * few extra special \@ ^~ ... etc are present for specific demo purpose
 */
static const uint8_t ascii_to_display_lut[256]           =
{
   [' ']                                                 = 0,
   ['-']                                                 = S6,
   ['_']                                                 = S3,
   ['=']                                                 = S3 + S6,
   ['~']                                                 = S0 + S3 + S6,                           // 3 h bar
   ['^']                                                 = S0,                                     // use as top bar

   ['?']                                                 = NOT_7_NO_DP(S5 + S3 + S2),
   ['*']                                                 = NOT_7_NO_DP(),
   ['[']                                                 = S0 + S3 + S4 + S5,
   [']']                                                 = S0 + S3 + S2 + S1,
   ['@']                                                 = S0 + S3,

   ['0']                                                 = NOT_7_NO_DP(S6),
   ['1']                                                 = S1 + S2,
   ['2']                                                 = S0 + S1 + S6 + S4 + S3,
   ['3']                                                 = NOT_7_NO_DP(S4 + S5),
   ['4']                                                 = S5 + S1 + S6 + S2,
   ['5']                                                 = NOT_7_NO_DP(S1 + S4),
   ['6']                                                 = NOT_7_NO_DP(S1),
   ['7']                                                 = S0 + S1 + S2,
   ['8']                                                 = NOT_7_NO_DP(0),
   ['9']                                                 = NOT_7_NO_DP(S4),

   ['a']                                                 = S2 + S3 + S4 + S6 ,
   ['b']                                                 = NOT_7_NO_DP(S0 + S1),
   ['c']                                                 = S6 + S4 + S3,
   ['d']                                                 = NOT_7_NO_DP(S0 + S5),
   ['e']                                                 = NOT_7_NO_DP(S2),
   ['f']                                                 = S6 + S5 + S4 + S0,                      // same as F
   ['g']                                                 = NOT_7_NO_DP(S4),                        // same as 9
   ['h']                                                 = S6 + S5 + S4 + S2,
   ['i']                                                 = S4,
   ['j']                                                 = S1 + S2 + S3 + S4,
   ['k']                                                 = S6 + S5 + S4 + S2,                      // a h
   ['l']                                                 = S3 + S4,
   ['m']                                                 = S0 + S4 + S2,                           // same as
   ['n']                                                 = S2 + S4 + S6,
   ['o']                                                 = S6 + S4 + S3 + S2,
   ['p']                                                 = NOT_7_NO_DP(S3 + S2),                   // same as P
   ['q']                                                 = S0 + S1 + S2 + S5 + S6,
   ['r']                                                 = S4 + S6,
   ['s']                                                 = NOT_7_NO_DP(S1 + S4),
   ['t']                                                 = NOT_7_NO_DP(S0 + S1 + S2),
   ['u']                                                 = S4 + S3 + S2 + S5 + S1,                 // U
   ['v']                                                 = S4 + S3 + S2,                           // is u but u use U
   ['w']                                                 = S1 + S3 + S5,
   ['x']                                                 = NOT_7_NO_DP(S0 + S3),                   // similar to H
   ['y']                                                 = NOT_7_NO_DP(S0 + S4),
   ['z']                                                 = S0 + S1 + S6 + S4 + S3,                 // same as 2

   ['A']                                                 = NOT_7_NO_DP(S3),
   ['B']                                                 = NOT_7_NO_DP(S0 + S1),                   // as b
   ['C']                                                 = S0 + S3 + S4 + S5,                      // same as [
   ['E']                                                 = NOT_7_NO_DP(S1 + S2),
   ['F']                                                 = S6 + S5 + S4 + S0,
   ['G']                                                 = NOT_7_NO_DP(S4),                        // same as 9
   ['H']                                                 = NOT_7_NO_DP(S0 + S3),
   ['I']                                                 = S1 + S2,
   ['J']                                                 = S1 + S2 + S3 + S4,
   ['K']                                                 = NOT_7_NO_DP(S0 + S3),                   // same as H
   ['L']                                                 = S3 + S4 + S5,
   ['M']                                                 = S0 + S4 + S2,                           // same as m
   ['N']                                                 = S2 + S4 + S6,                           // same as n
   ['O']                                                 = NOT_7_NO_DP(S6),
   ['P']                                                 = NOT_7_NO_DP(S3 + S2),
   ['Q']                                                 = NOT_7_NO_DP(S3 + S2),
   ['R']                                                 = S4 + S6,
   ['S']                                                 = NOT_7_NO_DP(S1 + S4),                   // sasme as 5
   ['T']                                                 = NOT_7_NO_DP(S0 + S1 + S2),              // sasme as t
   ['U']                                                 = NOT_7_NO_DP(S6 + S0),
   ['V']                                                 = S4 + S3 + S2,                           // is u but u use U
   ['W']                                                 = S1 + S3 + S5,
   ['X']                                                 = NOT_7_NO_DP(S0 + S3),                   // similar to H
   ['Y']                                                 = NOT_7_NO_DP(S0 + S4),
   ['Z']                                                 = S0 + S1 + S6 + S4 + S3,                 // same as 2
};

static union CurIOVal_u
{
   uint8_t bytes[4];                                                          // 4 bytes array i/o view
   uint32_t u32;                                                              // single dword i/o view
}
// cache the extended IO values
CurIOVal;

static int DisplayBitPos[4]                              = {0, 7, 16, 16 + 7};


#ifndef XNUCLEO53L0A1_GetI2cBus
#if defined(RTOS_FREERTOS)
static void XNUCLEO53L0A1_GetI2cBus()
{
//   xSemaphoreTake(*g_i2c_semaphore, portMAX_DELAY);
}
#else
#define XNUCLEO53L0A1_GetI2cBus(...)                     (void) 0
#endif   // RTOS_FREERTOS
#endif   // XNUCLEO53L0A1_GetI2cBus

#ifndef XNUCLEO53L0A1_PutI2cBus
#if defined(RTOS_FREERTOS)
static void XNUCLEO53L0A1_PutI2cBus()
{
//   xSemaphoreGive(*g_i2c_semaphore);
}
#else
#define XNUCLEO53L0A1_PutI2cBus(...)                     (void) 0
#endif
#endif   // XNUCLEO53L0A1_PutI2cBus

static int _ExpanderRd(int I2cExpAddr, int index, uint8_t *data, int n_data);
static int _ExpanderWR(int I2cExpAddr, int index, uint8_t *data, int n_data);
static int _ExpandersSetAllIO(void);

/* --------------------------------------------------------------------------
 * Name : _ExpandersSetAllIO()
 *
 *
 * -------------------------------------------------------------------------- */
int XNUCLEO53L0A1_SetDisplayString(const char *str)
{
   int status;
   uint32_t Segments;
   int BitPos;
   int i;

   for (i = 0; i < 4 && str[i] != 0; i++)
   {
      Segments                                           = (uint32_t) ascii_to_display_lut[(uint8_t) str[i]];
      Segments                                           = (~Segments) & 0x7F;
      BitPos                                             = DisplayBitPos[i];
      CurIOVal.u32                                       &= ~(0x7F << BitPos);
      CurIOVal.u32                                       |= Segments << BitPos;
   }
   // clear unused digit
   for ( ; i < 4; i++)
   {
      BitPos                                             = DisplayBitPos[i];
      CurIOVal.u32                                       |= 0x7F << BitPos;
   }
   status                                                = _ExpandersSetAllIO();
   if (status)
   {
//      XNUCLEO53L0A1_ErrLog("Set i/o");
   }
   return status;
}

/* --------------------------------------------------------------------------
 * Name : _ExpandersSetAllIO()
 *
 *
 * -------------------------------------------------------------------------- */
static int _ExpandersSetAllIO(void)
{
   int status;
   status                                                = _ExpanderWR(I2cExpAddr0, STMPE1600_GPSR, &CurIOVal.bytes[0], 2);
   if( status )
   {
      return status;
   }
   status                                                = _ExpanderWR(I2cExpAddr1, STMPE1600_GPSR, &CurIOVal.bytes[2], 2);
   return status;
}

/* --------------------------------------------------------------------------
 * Name : _ExpanderRd()
 *
 *
 * -------------------------------------------------------------------------- */
static int _ExpanderRd(int I2cExpAddr, int index, uint8_t *data, int n_data)
{
   int status;
   uint8_t RegAddr;
   RegAddr                                               = index;

   XNUCLEO53L0A1_GetI2cBus();
   do
   {
      status                                             = HAL_I2C_Master_Transmit(g_i2c_handle, I2cExpAddr, &RegAddr, 1, 100);
      if (status)
      {
         break;
      }
      status                                             = HAL_I2C_Master_Receive(g_i2c_handle, I2cExpAddr, data, n_data, n_data * 100);
   } while (0);
   XNUCLEO53L0A1_PutI2cBus();
   return status;
}

/* --------------------------------------------------------------------------
 * Name : _ExpanderRd()
 *
 *
 * -------------------------------------------------------------------------- */
static int _ExpanderWR(int I2cExpAddr, int index, uint8_t *data, int n_data)
{
   int status;
   uint8_t RegAddr[0x10];

   RegAddr[0]                                            = index;
   memcpy(RegAddr + 1, data, n_data);
   XNUCLEO53L0A1_GetI2cBus();
   status                                                = HAL_I2C_Master_Transmit(g_i2c_handle, I2cExpAddr, RegAddr, n_data + 1, 100);
   XNUCLEO53L0A1_PutI2cBus();
   return status;
}

#endif

/* --------------------------------------------------------------------------
 * Name : XNUCLEO53L1A1_SetDisplayString()
 *
 *
 * -------------------------------------------------------------------------- */
int XNUCLEO53L1A1_SetDisplayString(const char *str)
{
   int status                                            = 0;
   uint32_t Segments                                     = 0;
   int BitPos                                            = 0;
   int i                                                 = 0;

   for (i = 0; i < 4 && str[i] != 0; i++)
   {
      Segments                                           = (uint32_t) ascii_to_display_lut[(uint8_t) str[i]];
      Segments                                           = (~Segments) & 0x7F;
      BitPos                                             = DisplayBitPos[i];
      CurIOVal.u32                                       &= ~(0x7F << BitPos);
      CurIOVal.u32                                       |= Segments<<BitPos;
   }
   // clear unused digit
   for (; i < 4; i++)
   {
      BitPos                                             = DisplayBitPos[i];
      CurIOVal.u32                                       |= 0x7F << BitPos;
   }
   status                                                = _ExpandersSetAllIO();
   if (status)
   {
      debug_output_error("Set i/o \r\n");
   }
   return status;
}


/* --------------------------------------------------------------------------
 * Name : gpio_stmpe1600_reset()
 *
 *
 * -------------------------------------------------------------------------- */
void gpio_stmpe1600_reset(enum XNUCLEO53L1A1_dev_e DevNo, int state)
{
   int status                                            = 0;

   switch (DevNo)
   {
      case XNUCLEO53L1A1_DEV_CENTER :
         CurIOVal.bytes[3]                               &= (~0x80);       // bit 15 expender 1  => byte #3
         if (state)
         {
            CurIOVal.bytes[3]                            |= 0x80;          // bit 15 expender 1  => byte #3
         }
         status                                          = _ExpanderWR(I2cExpAddr1, STMPE1600_GPSR + 1, &CurIOVal.bytes[3], 1);
         break;

      case XNUCLEO53L1A1_DEV_LEFT :
         CurIOVal.bytes[1]                               &= (~0x40);       // bit 14 expender 0 => byte #1
         if (state)
         {
            CurIOVal.bytes[1]                            |= 0x40;          // bit 14 expender 0 => byte #1
         }
         status                                          = _ExpanderWR(I2cExpAddr0, STMPE1600_GPSR + 1, &CurIOVal.bytes[1], 1);
         break;

      case XNUCLEO53L1A1_DEV_RIGHT :
         CurIOVal.bytes[1]                               &= (~0x80);       // bit 15 expender 0  => byte #1
         if (state)
         {
            CurIOVal.bytes[1]                            |= 0x80;          // bit 15 expender 0 => byte #1
         }
         status                                          = _ExpanderWR(I2cExpAddr0, STMPE1600_GPSR + 1, &CurIOVal.bytes[1], 1);
         break;

      default :
         debug_output_error("Invalid DevNo %d  \r\n", DevNo);
         status                                          = -1;
         break;
   }
   //error with valid id
   if (status)
   {
      debug_output_error("expander i/o error for DevNo %d state %d  \r\n",DevNo, state);
   }
}


/* --------------------------------------------------------------------------
 * Name : gpio_stmpe1600_init()
 *
 *
 * -------------------------------------------------------------------------- */
void gpio_stmpe1600_init(void* pI2C_Handle, void* bus_semaphore)
{
   int status                                            = 0;
   uint8_t ExpanderData[2]                               = {0, };
   g_i2c_handle                                          = (I2C_HandleTypeDef *) pI2C_Handle;

#if defined(RTOS_FREERTOS)
   g_i2c_semaphore                                       = bus_semaphore;
#endif

   status                                                = _ExpanderRd(I2cExpAddr0, STMPE1600_CHIP_ID_LSB_REG, ExpanderData, 2);
   if (status != 0 || ExpanderData[0] != STMPE1600_CHIP_ID_LSB || ExpanderData[1] != STMPE1600_CHIP_ID_MSB)
   {
      debug_output_error("I2C Expander @0x%02X not detected \r\n", (int) I2cExpAddr0);
   }
   debug_output_info("Found STMPE1600 gpio extension 1 \r\n");

   status                                                = _ExpanderRd(I2cExpAddr1, STMPE1600_CHIP_ID_LSB_REG, ExpanderData, 2);
   if (status != 0 || ExpanderData[0] != STMPE1600_CHIP_ID_LSB || ExpanderData[1] != STMPE1600_CHIP_ID_MSB)
   {
      debug_output_error("I2C Expander @0x%02X not detected \r\n", (int) I2cExpAddr0);
   }
   debug_output_info("Found STMPE1600 gpio extension 2 \r\n");


   CurIOVal.u32                                          = 0x0;
   // setup expender i/o direction  all output but exp1 bit 14
   ExpanderData[0]                                       = 0xFF;
   ExpanderData[1]                                       = 0xFF;
   status                                                = _ExpanderWR(I2cExpAddr0, STMPE1600_GPDR, ExpanderData, 2);

   if (status)
   {
      debug_output_error("Set Expander @0x%02X DR \r\n", I2cExpAddr0);
   }
   ExpanderData[0]                                       = 0xFF;
   ExpanderData[1]                                       = 0xBF;                    // all but bit 14-15 that is pb1 and xhurt
   status                                                = _ExpanderWR(I2cExpAddr1, STMPE1600_GPDR, ExpanderData, 2);
   if (status)
   {
      debug_output_error("Set Expander @0x%02X DR \r\n", I2cExpAddr1);
   }

   // shut down all segment and all device
   CurIOVal.u32                                          = 0x7F + (0x7F << 7) + (0x7F << 16) + (0x7F << (16 + 7));
   status                                                = _ExpandersSetAllIO();
   if (status)
   {
      debug_output_error("Set initial i/o  \r\n");
   }
}



