/*
 * File: touch_ft5536.c
 *
 * Written by duvallee in 2018
 *
 */
#include <stdio.h>
#include "main.h"
#include "touch_ft5536.h"


// ============================================================================
#define FT5536_I2C_SLAVE_ADDRESS                         0x70

// ============================================================================
//
// @brief : Definitions for FT5336 I2C register addresses on 8 bit
//

// ----------------------------------------------------------------------------
// Current mode register of the FT5336 (R/W)
#define FT5336_DEV_MODE_REG                              ((uint8_t) 0x00)

#define FT5336_DEV_MODE_MASK                             ((uint8_t) 0x70)

// Possible values of FT5336_DEV_MODE_REG
#define FT5336_DEV_MODE_WORKING                          ((uint8_t) 0x00)
#define FT5336_DEV_MODE_FACTORY                          ((uint8_t) 0x40)


// ----------------------------------------------------------------------------
// Gesture ID register(maybe read only ...)
#define FT5336_GEST_ID_REG                               ((uint8_t) 0x01)

// Possible values of FT5336_GEST_ID_REG
#define FT5336_GEST_ID_NO_GESTURE                        ((uint8_t) 0x00)
#define FT5336_GEST_ID_MOVE_UP                           ((uint8_t) 0x10)
#define FT5336_GEST_ID_MOVE_RIGHT                        ((uint8_t) 0x14)
#define FT5336_GEST_ID_MOVE_DOWN                         ((uint8_t) 0x18)
#define FT5336_GEST_ID_MOVE_LEFT                         ((uint8_t) 0x1C)
#define FT5336_GEST_ID_SINGLE_CLICK                      ((uint8_t) 0x20)
#define FT5336_GEST_ID_DOUBLE_CLICK                      ((uint8_t) 0x22)
#define FT5336_GEST_ID_ROTATE_CLOCKWISE                  ((uint8_t) 0x28)
#define FT5336_GEST_ID_ROTATE_C_CLOCKWISE                ((uint8_t) 0x29)
#define FT5336_GEST_ID_ZOOM_IN                           ((uint8_t) 0x40)
#define FT5336_GEST_ID_ZOOM_OUT                          ((uint8_t) 0x49)

// ----------------------------------------------------------------------------
// Touch Data Status register : gives number of active touch points (0..5) */
#define FT5336_TD_STAT_REG                               ((uint8_t) 0x02)

// Values related to FT5336_TD_STAT_REG
#define FT5336_TD_STAT_MASK                              ((uint8_t) 0x0F)
// Max detectable simultaneous touches
#define FT5336_MAX_DETECTABLE_TOUCH                      ((uint8_t) 0x05)

// ----------------------------------------------------------------------------
// coordinate for P1
#define FT5336_P1_XH_REG                                 ((uint8_t) 0x03)
#define FT5336_P1_XL_REG                                 ((uint8_t) 0x04)
#define FT5336_P1_YH_REG                                 ((uint8_t) 0x05)
#define FT5336_P1_YL_REG                                 ((uint8_t) 0x06)
// Touch Pressure register value (R)
#define FT5336_P1_WEIGHT_REG                             ((uint8_t) 0x07)
// Touch area register
#define FT5336_P1_MISC_REG                               ((uint8_t) 0x08)

// Values Pn_XH and Pn_YH related
// xx?? ???? (msb 2 bits of XH register)
#define FT5336_TOUCH_EVT_FLAG_MASK                       ((uint8_t) 0xC0)

#define FT5336_TOUCH_EVT_FLAG_PRESS_DOWN                 ((uint8_t) 0x00)
#define FT5336_TOUCH_EVT_FLAG_LIFT_UP                    ((uint8_t) 0x40)
#define FT5336_TOUCH_EVT_FLAG_CONTACT                    ((uint8_t) 0x80)
#define FT5336_TOUCH_EVT_FLAG_NO_EVENT                   ((uint8_t) 0xC0)

// ???? xxxx (lsb 4 bits of XH register)
// x/y-pos : ???? xxxx (XH) - xxxx xxxx (XL) - total 12-bits (4096)
#define FT5336_TOUCH_POS_MSB_MASK                        ((uint8_t) 0x0F)
// XL register
#define FT5336_TOUCH_POS_LSB_MASK                        ((uint8_t) 0xFF)

// Values Pn_WEIGHT related
#define FT5336_TOUCH_WEIGHT_MASK                         ((uint8_t) 0xFF)

// Values related to FT5336_Pn_MISC_REG
#define FT5336_TOUCH_AREA_MASK                           ((uint8_t) 0x40)
#define FT5336_TOUCH_AREA_SHIFT                          ((uint8_t) 0x04)

// ----------------------------------------------------------------------------
#define FT5336_P2_XH_REG                                 ((uint8_t) 0x09)
#define FT5336_P2_XL_REG                                 ((uint8_t) 0x0A)
#define FT5336_P2_YH_REG                                 ((uint8_t) 0x0B)
#define FT5336_P2_YL_REG                                 ((uint8_t) 0x0C)
#define FT5336_P2_WEIGHT_REG                             ((uint8_t) 0x0D)
#define FT5336_P2_MISC_REG                               ((uint8_t) 0x0E)

// ----------------------------------------------------------------------------
#define FT5336_P3_XH_REG                                 ((uint8_t) 0x0F)
#define FT5336_P3_XL_REG                                 ((uint8_t) 0x10)
#define FT5336_P3_YH_REG                                 ((uint8_t) 0x11)
#define FT5336_P3_YL_REG                                 ((uint8_t) 0x12)
#define FT5336_P3_WEIGHT_REG                             ((uint8_t) 0x13)
#define FT5336_P3_MISC_REG                               ((uint8_t) 0x14)

// ----------------------------------------------------------------------------
#define FT5336_P4_XH_REG                                 ((uint8_t) 0x15)
#define FT5336_P4_XL_REG                                 ((uint8_t) 0x16)
#define FT5336_P4_YH_REG                                 ((uint8_t) 0x17)
#define FT5336_P4_YL_REG                                 ((uint8_t) 0x18)
#define FT5336_P4_WEIGHT_REG                             ((uint8_t) 0x19)
#define FT5336_P4_MISC_REG                               ((uint8_t) 0x1A)

// ----------------------------------------------------------------------------
#define FT5336_P5_XH_REG                                 ((uint8_t) 0x1B)
#define FT5336_P5_XL_REG                                 ((uint8_t) 0x1C)
#define FT5336_P5_YH_REG                                 ((uint8_t) 0x1D)
#define FT5336_P5_YL_REG                                 ((uint8_t) 0x1E)
#define FT5336_P5_WEIGHT_REG                             ((uint8_t) 0x1F)
#define FT5336_P5_MISC_REG                               ((uint8_t) 0x20)

// ----------------------------------------------------------------------------
#define FT5336_P6_XH_REG                                 ((uint8_t) 0x21)
#define FT5336_P6_XL_REG                                 ((uint8_t) 0x22)
#define FT5336_P6_YH_REG                                 ((uint8_t) 0x23)
#define FT5336_P6_YL_REG                                 ((uint8_t) 0x24)
#define FT5336_P6_WEIGHT_REG                             ((uint8_t) 0x25)
#define FT5336_P6_MISC_REG                               ((uint8_t) 0x26)

// ----------------------------------------------------------------------------
#define FT5336_P7_XH_REG                                 ((uint8_t) 0x27)
#define FT5336_P7_XL_REG                                 ((uint8_t) 0x28)
#define FT5336_P7_YH_REG                                 ((uint8_t) 0x29)
#define FT5336_P7_YL_REG                                 ((uint8_t) 0x2A)
#define FT5336_P7_WEIGHT_REG                             ((uint8_t) 0x2B)
#define FT5336_P7_MISC_REG                               ((uint8_t) 0x2C)

// ----------------------------------------------------------------------------
#define FT5336_P8_XH_REG                                 ((uint8_t) 0x2D)
#define FT5336_P8_XL_REG                                 ((uint8_t) 0x2E)
#define FT5336_P8_YH_REG                                 ((uint8_t) 0x2F)
#define FT5336_P8_YL_REG                                 ((uint8_t) 0x30)
#define FT5336_P8_WEIGHT_REG                             ((uint8_t) 0x31)
#define FT5336_P8_MISC_REG                               ((uint8_t) 0x32)

// ----------------------------------------------------------------------------
#define FT5336_P9_XH_REG                                 ((uint8_t) 0x33)
#define FT5336_P9_XL_REG                                 ((uint8_t) 0x34)
#define FT5336_P9_YH_REG                                 ((uint8_t) 0x35)
#define FT5336_P9_YL_REG                                 ((uint8_t) 0x36)
#define FT5336_P9_WEIGHT_REG                             ((uint8_t) 0x37)
#define FT5336_P9_MISC_REG                               ((uint8_t) 0x38)

// ----------------------------------------------------------------------------
#define FT5336_P10_XH_REG                                ((uint8_t) 0x39)
#define FT5336_P10_XL_REG                                ((uint8_t) 0x3A)
#define FT5336_P10_YH_REG                                ((uint8_t) 0x3B)
#define FT5336_P10_YL_REG                                ((uint8_t) 0x3C)
#define FT5336_P10_WEIGHT_REG                            ((uint8_t) 0x3D)
#define FT5336_P10_MISC_REG                              ((uint8_t) 0x3E)

// ----------------------------------------------------------------------------
// Threshold for touch detection
#define FT5336_TH_GROUP_REG                              ((uint8_t) 0x80)

// Values FT5336_TH_GROUP_REG : threshold related
#define FT5336_THRESHOLD_MASK                            ((uint8_t) 0xFF)

// ----------------------------------------------------------------------------
// Filter function coefficients
#define FT5336_TH_DIFF_REG                               ((uint8_t) 0x85)

// ----------------------------------------------------------------------------
// Control register
#define FT5336_CTRL_REG                                  ((uint8_t) 0x86)

// Values related to FT5336_CTRL_REG
// Will keep the Active mode when there is no touching
#define FT5336_CTRL_KEEP_ACTIVE_MODE                     ((uint8_t) 0x00)
// Switching from Active mode to Monitor mode automatically when there is no touching
#define FT5336_CTRL_KEEP_AUTO_SWITCH_MONITOR_MODE        ((uint8_t) 0x01)

// ----------------------------------------------------------------------------
// The time period of switching from Active mode to Monitor mode when there is no touching
#define FT5336_TIMEENTERMONITOR_REG                      ((uint8_t) 0x87)

// ----------------------------------------------------------------------------
// Report rate in Active mode
#define FT5336_PERIODACTIVE_REG                          ((uint8_t) 0x88)

// ----------------------------------------------------------------------------
// Report rate in Monitor mode
#define FT5336_PERIODMONITOR_REG                         ((uint8_t) 0x89)

// ----------------------------------------------------------------------------
// The value of the minimum allowed angle while Rotating gesture mode
#define FT5336_RADIAN_VALUE_REG                          ((uint8_t) 0x91)

// ----------------------------------------------------------------------------
// Maximum offset while Moving Left and Moving Right gesture
#define FT5336_OFFSET_LEFT_RIGHT_REG                     ((uint8_t) 0x92)

// ----------------------------------------------------------------------------
// Maximum offset while Moving Up and Moving Down gesture
#define FT5336_OFFSET_UP_DOWN_REG                        ((uint8_t) 0x93)

// ----------------------------------------------------------------------------
// Minimum distance while Moving Left and Moving Right gesture
#define FT5336_DISTANCE_LEFT_RIGHT_REG                   ((uint8_t) 0x94)

// ----------------------------------------------------------------------------
// Minimum distance while Moving Up and Moving Down gesture
#define FT5336_DISTANCE_UP_DOWN_REG                      ((uint8_t) 0x95)

// ----------------------------------------------------------------------------
// Maximum distance while Zoom In and Zoom Out gesture
#define FT5336_DISTANCE_ZOOM_REG                         ((uint8_t) 0x96)

// ----------------------------------------------------------------------------
// High 8-bit of LIB Version info
#define FT5336_LIB_VER_H_REG                             ((uint8_t) 0xA1)

// ----------------------------------------------------------------------------
// Low 8-bit of LIB Version info
#define FT5336_LIB_VER_L_REG                             ((uint8_t) 0xA2)

// ----------------------------------------------------------------------------
// Chip Selecting
#define FT5336_CIPHER_REG                                ((uint8_t) 0xA3)

// ----------------------------------------------------------------------------
// Interrupt mode register (used when in interrupt mode)
#define FT5336_GMODE_REG                                 ((uint8_t) 0xA4)

#define FT5336_G_MODE_INTERRUPT_MASK                     ((uint8_t) 0x03)

// Possible values of FT5336_GMODE_REG
#define FT5336_G_MODE_INTERRUPT_POLLING                  ((uint8_t) 0x00)
#define FT5336_G_MODE_INTERRUPT_TRIGGER                  ((uint8_t) 0x01)

// ----------------------------------------------------------------------------
// Current power mode the FT5336 system is in (R)
#define FT5336_PWR_MODE_REG                              ((uint8_t) 0xA5)

// ----------------------------------------------------------------------------
// FT5336 firmware version
#define FT5336_FIRMID_REG                                ((uint8_t) 0xA6)

// ----------------------------------------------------------------------------
// FT5336 Chip identification register
#define FT5336_CHIP_ID_REG                               ((uint8_t) 0xA8)

//  Possible values of FT5336_CHIP_ID_REG
#define FT5336_ID_VALUE                                  ((uint8_t) 0x51)

// ----------------------------------------------------------------------------
// Release code version
#define FT5336_RELEASE_CODE_ID_REG                       ((uint8_t) 0xAF)

// ----------------------------------------------------------------------------
// Current operating mode the FT5336 system is in (R)
#define FT5336_STATE_REG                                 ((uint8_t) 0xBC)

// ---------------------------------------------------------------------------
// global variable
static TOUCH_FT5536_STRUCT g_Touch_FT5536_struct;
static TOUCH_EVENT_STRUCT g_Touch_Event;


/* --------------------------------------------------------------------------
 * Name : touch_read_reg()
 *
 *
 * -------------------------------------------------------------------------- */
static int touch_read_reg(uint8_t Reg, uint8_t* pvalue)
{
   if (g_Touch_FT5536_struct.read_fn(FT5536_I2C_SLAVE_ADDRESS, (uint16_t) Reg, sizeof(uint8_t), pvalue, sizeof(uint8_t)) < 0)
   {
      return -1;
   }

   return 0;
}

/* --------------------------------------------------------------------------
 * Name : touch_write_reg()
 *
 *
 * -------------------------------------------------------------------------- */
static int touch_write_reg(uint8_t Reg, uint8_t value)
{
   if (g_Touch_FT5536_struct.read_fn(FT5536_I2C_SLAVE_ADDRESS, (uint16_t) Reg, sizeof(uint8_t), &value, sizeof(uint8_t)) < 0)
   {
      return -1;
   }

   return 0;
}

/* --------------------------------------------------------------------------
 * Name : get_touch_point()
 *
 *
 * -------------------------------------------------------------------------- */
#define XH_REG_INDEX                                     0
#define XL_REG_INDEX                                     1
#define YH_REG_INDEX                                     2
#define YL_REG_INDEX                                     3
#define WEIGHT_REG_INDEX                                 4
#define MISC_REG_INDEX                                   5
static void get_touch_point(uint8_t start_reg_addr, TOUCH_POINT_STRUCT* pPoint)
{
   uint8_t reg_value                                     = 0;
   uint8_t reg_hi_value                                  = 0;
   uint8_t reg_low_value                                 = 0;

   touch_read_reg(start_reg_addr + XH_REG_INDEX, &reg_hi_value);
   touch_read_reg(start_reg_addr + XL_REG_INDEX, &reg_low_value);

   pPoint->touch_action                                  = (reg_hi_value & FT5336_TOUCH_EVT_FLAG_MASK);

   pPoint->y_pos                                         = (((reg_hi_value & FT5336_TOUCH_POS_MSB_MASK) << 8) | reg_low_value);

   touch_read_reg(start_reg_addr + YH_REG_INDEX, &reg_hi_value);
   touch_read_reg(start_reg_addr + YL_REG_INDEX, &reg_low_value);

   pPoint->x_pos                                         = (((reg_hi_value & FT5336_TOUCH_POS_MSB_MASK) << 8) | reg_low_value);

   touch_read_reg(start_reg_addr + WEIGHT_REG_INDEX, &reg_value);
   pPoint->touch_weight                                  = reg_value;

   touch_read_reg(start_reg_addr + MISC_REG_INDEX, &reg_value);
   pPoint->touch_area                                    = ((reg_value & FT5336_TOUCH_AREA_MASK) >> FT5336_TOUCH_AREA_SHIFT);

}



/* --------------------------------------------------------------------------
 * Name : touch_ft5536_init()
 *        touch init function
 *
 * -------------------------------------------------------------------------- */
void touch_ft5536_init(FT5536_READ_FN readfn, FT5536_WRITE_FN writefn)
{
   uint8_t reg_value                                     = 0;
   uint8_t reg_ex_value                                  = 0;

   g_Touch_FT5536_struct.read_fn                         = readfn;
   g_Touch_FT5536_struct.write_fn                        = writefn;

   HAL_Delay(1);

   debug_output_dump("==================================================== \r\n");
   debug_output_dump("FocalTech FT5336GQQ - CTP Controller \r\n\r\n");
   if (touch_read_reg(FT5336_CHIP_ID_REG, &reg_value) == HAL_OK)
   {
      debug_output_dump("CHIP ID(0x%02X)        : 0x%02X \r\n", FT5336_ID_VALUE, reg_value);
   }
   if (touch_read_reg(FT5336_RELEASE_CODE_ID_REG, &reg_value) == HAL_OK)
   {
      debug_output_dump("RELEASE CODE ID      : 0x%02X \r\n", reg_value);
   }
   if (touch_read_reg(FT5336_STATE_REG, &reg_value) == HAL_OK)
   {
      debug_output_dump("CURRENT OP MODE      : 0x%02X \r\n", reg_value);
   }
   if (touch_read_reg(FT5336_FIRMID_REG, &reg_value) == HAL_OK)
   {
      debug_output_dump("F/W VER              : 0x%02X \r\n", reg_value);
   }

   if (touch_read_reg(FT5336_LIB_VER_L_REG, &reg_value) == HAL_OK)
   {
   }
   if (touch_read_reg(FT5336_LIB_VER_H_REG, &reg_ex_value) == HAL_OK)
   {
      debug_output_dump("LIB VER VER          : 0x%02X%02X \r\n", reg_ex_value, reg_value);
   }

   if (touch_read_reg(FT5336_GMODE_REG, &reg_value) == HAL_OK)
   {
      debug_output_dump("IRQ MODE             : 0x%02X \r\n", reg_value);
   }

   if (touch_read_reg(FT5336_CIPHER_REG, &reg_value) == HAL_OK)
   {
      debug_output_dump("CHIP SELECT          : 0x%02X \r\n", reg_value);
   }

   if (touch_read_reg(FT5336_PERIODMONITOR_REG, &reg_value) == HAL_OK)
   {
      debug_output_dump("rate in Mointor mode : 0x%02X \r\n", reg_value);
   }

   if (touch_read_reg(FT5336_TIMEENTERMONITOR_REG, &reg_value) == HAL_OK)
   {
      debug_output_dump("time period          : 0x%02X \r\n", reg_value);
   }

   if (touch_read_reg(FT5336_CTRL_REG, &reg_value) == HAL_OK)
   {
      debug_output_dump("time period          : 0x%02X \r\n", reg_value);
   }

   if (touch_read_reg(FT5336_DEV_MODE_REG, &reg_value) == HAL_OK)
   {
      debug_output_dump("Device Mode          : %s \r\n", ((reg_value & FT5336_DEV_MODE_MASK) == FT5336_DEV_MODE_FACTORY) ? "Factory Mode" : "Working Mode");
   }
   debug_output_dump("==================================================== \r\n");
}


/* --------------------------------------------------------------------------
 * Name : touch_start()
 *
 *
 * -------------------------------------------------------------------------- */
void touch_start(TOUCH_FT5536_TRIGGER_MODE trigger_mode)
{
   if (touch_write_reg(FT5336_GMODE_REG, trigger_mode & FT5336_G_MODE_INTERRUPT_MASK) != HAL_OK)
   {
      debug_output_error("touch_start() Failed \r\n");
      return;
   }
}

/* --------------------------------------------------------------------------
 * Name : get_touch_event()
 *
 *
 * -------------------------------------------------------------------------- */
uint8_t get_touch_event()
{
   uint8_t reg_value                                     = 0;
   uint8_t touch_pt_num                                  = 0;

   touch_read_reg(FT5336_TD_STAT_REG, &reg_value);

   touch_pt_num                                          = (reg_value & FT5336_TD_STAT_MASK);
   if (touch_pt_num == 0 || touch_pt_num > FT5336_MAX_DETECTABLE_TOUCH)
   {
      g_Touch_Event.touch_point_num                      = 0;
      return 0;
   }
   g_Touch_Event.touch_point_num                         = touch_pt_num;

   switch(g_Touch_Event.touch_point_num)
   {
      case 1 :
         get_touch_point(FT5336_P1_XH_REG, &(g_Touch_Event.touch_point[0]));
         break;

      case 2 :
         get_touch_point(FT5336_P1_XH_REG, &(g_Touch_Event.touch_point[0]));
         get_touch_point(FT5336_P2_XH_REG, &(g_Touch_Event.touch_point[1]));
         break;

      case 3 :
         get_touch_point(FT5336_P1_XH_REG, &(g_Touch_Event.touch_point[0]));
         get_touch_point(FT5336_P2_XH_REG, &(g_Touch_Event.touch_point[1]));
         get_touch_point(FT5336_P3_XH_REG, &(g_Touch_Event.touch_point[2]));
         break;

      case 4 :
         get_touch_point(FT5336_P1_XH_REG, &(g_Touch_Event.touch_point[0]));
         get_touch_point(FT5336_P2_XH_REG, &(g_Touch_Event.touch_point[1]));
         get_touch_point(FT5336_P3_XH_REG, &(g_Touch_Event.touch_point[2]));
         get_touch_point(FT5336_P4_XH_REG, &(g_Touch_Event.touch_point[3]));
         break;

      case 5 :
         get_touch_point(FT5336_P1_XH_REG, &(g_Touch_Event.touch_point[0]));
         get_touch_point(FT5336_P2_XH_REG, &(g_Touch_Event.touch_point[1]));
         get_touch_point(FT5336_P3_XH_REG, &(g_Touch_Event.touch_point[2]));
         get_touch_point(FT5336_P4_XH_REG, &(g_Touch_Event.touch_point[3]));
         get_touch_point(FT5336_P5_XH_REG, &(g_Touch_Event.touch_point[4]));
         break;
   }
   touch_read_reg(FT5336_GEST_ID_REG, &reg_value);
   g_Touch_Event.touch_gesture                           = reg_value;

   return touch_pt_num;
}


/* --------------------------------------------------------------------------
 * Name : touch_polling()
 *
 *
 * -------------------------------------------------------------------------- */
void touch_polling(void* pData)
{
   uint8_t touch_pt_num                                  = 0;
   int i;
   touch_pt_num                                          = get_touch_event();
   if (touch_pt_num > 0)
   {
      debug_output_dump("pt=%d, ", touch_pt_num);
      for (i = 0; i < g_Touch_Event.touch_point_num; i++)
      {
         debug_output_dump("[x=%3d, y=%3d, ", g_Touch_Event.touch_point[i].x_pos, g_Touch_Event.touch_point[i].y_pos);
         debug_output_dump("action=");
         switch (g_Touch_Event.touch_point[i].touch_action)
         {
            case FT5336_TOUCH_EVT_FLAG_PRESS_DOWN :
               debug_output_dump("press, ");
               break;

            case FT5336_TOUCH_EVT_FLAG_LIFT_UP :
               debug_output_dump("up, ");
               break;

            case FT5336_TOUCH_EVT_FLAG_CONTACT :
               debug_output_dump("contact, ");
               break;

            case FT5336_TOUCH_EVT_FLAG_NO_EVENT :
               debug_output_dump("no_event, ");
               break;
         }
         debug_output_dump("weight=%3d, ", g_Touch_Event.touch_point[i].touch_weight);
         debug_output_dump("misc=%d] ", g_Touch_Event.touch_point[i].touch_area);
      }
      debug_output_dump("\r\n");
   }
}








