/*
 * File: discovery_stm32f7_driver.c
 *
 * Written by duvallee in 2018
 *
 */
#include "main.h"
#include "discovery_stm32f7_driver.h"

// --------------------------------------------------------------------------
static SDRAM_HandleTypeDef g_SDRAM_handle;

/* --------------------------------------------------------------------------
 * Name : BSP_SDRAM_Initialization_sequence()
 *
 *
 * -------------------------------------------------------------------------- */
#define SDRAM_MODEREG_BURST_LENGTH_1                     ((uint16_t) 0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2                     ((uint16_t) 0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4                     ((uint16_t) 0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8                     ((uint16_t) 0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL              ((uint16_t) 0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED             ((uint16_t) 0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2                      ((uint16_t) 0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3                      ((uint16_t) 0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD            ((uint16_t) 0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED         ((uint16_t) 0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE             ((uint16_t) 0x0200) 
#define SDRAM_TIMEOUT                                    ((uint32_t) 0xFFFF)

static void BSP_SDRAM_Initialization_sequence(uint32_t RefreshCount)
{
   __IO uint32_t tmpmrd                                  = 0;
static FMC_SDRAM_CommandTypeDef Command;

   // Step 1: Configure a clock configuration enable command
   Command.CommandMode                                   = FMC_SDRAM_CMD_CLK_ENABLE;
   Command.CommandTarget                                 = FMC_SDRAM_CMD_TARGET_BANK1;
   Command.AutoRefreshNumber                             = 1;
   Command.ModeRegisterDefinition                        = 0;

   // Send the command
   HAL_SDRAM_SendCommand(&g_SDRAM_handle, &Command, SDRAM_TIMEOUT);

   // Step 2: Insert 100 us minimum delay
   // Inserted delay is equal to 1 ms due to systick time base unit (ms)
   HAL_Delay(1);

   // Step 3: Configure a PALL (precharge all) command
   Command.CommandMode                                   = FMC_SDRAM_CMD_PALL;
   Command.CommandTarget                                 = FMC_SDRAM_CMD_TARGET_BANK1;
   Command.AutoRefreshNumber                             = 1;
   Command.ModeRegisterDefinition                        = 0;

   // Send the command
   HAL_SDRAM_SendCommand(&g_SDRAM_handle, &Command, SDRAM_TIMEOUT);  

   // Step 4: Configure an Auto Refresh command
   Command.CommandMode                                   = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
   Command.CommandTarget                                 = FMC_SDRAM_CMD_TARGET_BANK1;
   Command.AutoRefreshNumber                             = 8;
   Command.ModeRegisterDefinition                        = 0;

   // Send the command
   HAL_SDRAM_SendCommand(&g_SDRAM_handle, &Command, SDRAM_TIMEOUT);

   // Step 5: Program the external memory mode register
   tmpmrd                                                = (uint32_t) SDRAM_MODEREG_BURST_LENGTH_1          |  \
                                                                      SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |  \
                                                                      SDRAM_MODEREG_CAS_LATENCY_2           |  \
                                                                      SDRAM_MODEREG_OPERATING_MODE_STANDARD |  \
                                                                      SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

   Command.CommandMode                                   = FMC_SDRAM_CMD_LOAD_MODE;
   Command.CommandTarget                                 = FMC_SDRAM_CMD_TARGET_BANK1;
   Command.AutoRefreshNumber                             = 1;
   Command.ModeRegisterDefinition                        = tmpmrd;

   // Send the command
   HAL_SDRAM_SendCommand(&g_SDRAM_handle, &Command, SDRAM_TIMEOUT);

   // Step 6: Set the refresh rate counter
   // Set the device refresh rate
   HAL_SDRAM_ProgramRefreshRate(&g_SDRAM_handle, RefreshCount); 
}

/* --------------------------------------------------------------------------
 * Name : MX_FMC_Init()
 *        FMC initialization function
 *
 * -------------------------------------------------------------------------- */
#define REFRESH_COUNT                                    ((uint32_t) 0x0603)     /* SDRAM refresh counter (100Mhz SD clock) */
static void BSP_SDRAM_Init(void)
{
   FMC_SDRAM_TimingTypeDef SdramTiming;

   // Perform the SDRAM1 memory initialization sequence
   g_SDRAM_handle.Instance                               = FMC_SDRAM_DEVICE;
   // hsdram1.Init
   g_SDRAM_handle.Init.SDBank                            = FMC_SDRAM_BANK1;
   g_SDRAM_handle.Init.ColumnBitsNumber                  = FMC_SDRAM_COLUMN_BITS_NUM_8;
   g_SDRAM_handle.Init.RowBitsNumber                     = FMC_SDRAM_ROW_BITS_NUM_12;
   g_SDRAM_handle.Init.MemoryDataWidth                   = FMC_SDRAM_MEM_BUS_WIDTH_16;
   g_SDRAM_handle.Init.InternalBankNumber                = FMC_SDRAM_INTERN_BANKS_NUM_4;
   g_SDRAM_handle.Init.CASLatency                        = FMC_SDRAM_CAS_LATENCY_2;
   g_SDRAM_handle.Init.WriteProtection                   = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
   g_SDRAM_handle.Init.SDClockPeriod                     = FMC_SDRAM_CLOCK_PERIOD_2;
   g_SDRAM_handle.Init.ReadBurst                         = FMC_SDRAM_RBURST_ENABLE;
   g_SDRAM_handle.Init.ReadPipeDelay                     = FMC_SDRAM_RPIPE_DELAY_0;
   // SdramTiming
   SdramTiming.LoadToActiveDelay                         = 2;
   SdramTiming.ExitSelfRefreshDelay                      = 7;
   SdramTiming.SelfRefreshTime                           = 4;
   SdramTiming.RowCycleDelay                             = 7;
   SdramTiming.WriteRecoveryTime                         = 2;
   SdramTiming.RPDelay                                   = 2;
   SdramTiming.RCDDelay                                  = 2;

   if (HAL_SDRAM_Init(&g_SDRAM_handle, &SdramTiming) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   BSP_SDRAM_Initialization_sequence(REFRESH_COUNT);
}

/* --------------------------------------------------------------------------
 * Name : Display_On()
 *
 *
 * -------------------------------------------------------------------------- */
void Display_On(int on)
{
   if (on)
   {
      HAL_GPIO_WritePin(DISPLAY_CONTROL_PORT, DISPLAY_CONTROL_PIN, GPIO_PIN_SET);            // Display on
      HAL_GPIO_WritePin(BACKLIGHT_CONTROL_PORT, BACKLIGHT_CONTROL_PIN, GPIO_PIN_SET);        // Backlight on
   }
   else
   {
      HAL_GPIO_WritePin(DISPLAY_CONTROL_PORT, DISPLAY_CONTROL_PIN, GPIO_PIN_RESET);          // Display off
      HAL_GPIO_WritePin(BACKLIGHT_CONTROL_PORT, BACKLIGHT_CONTROL_PIN, GPIO_PIN_RESET);      // Backlight off
   }
}

// ***************************************************************************
// Fuction      : Board_Driver_Init()
// Description  : 
// 
//
// ***************************************************************************
void Board_Driver_Init()
{
   GPIO_InitTypeDef GPIO_InitStruct;

   __HAL_RCC_GPIOI_CLK_ENABLE();
   __HAL_RCC_GPIOK_CLK_ENABLE();

   // Display Control
   GPIO_InitStruct.Pin                                   = DISPLAY_CONTROL_PIN;
   GPIO_InitStruct.Mode                                  = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull                                  = GPIO_NOPULL;
   GPIO_InitStruct.Speed                                 = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(DISPLAY_CONTROL_PORT, &GPIO_InitStruct);

   // Backlight Control
   GPIO_InitStruct.Pin                                   = BACKLIGHT_CONTROL_PIN;
   GPIO_InitStruct.Mode                                  = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull                                  = GPIO_NOPULL;
   GPIO_InitStruct.Speed                                 = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(BACKLIGHT_CONTROL_PORT, &GPIO_InitStruct);

   Display_On(0);

   // initialize sdram
   BSP_SDRAM_Init();
}



