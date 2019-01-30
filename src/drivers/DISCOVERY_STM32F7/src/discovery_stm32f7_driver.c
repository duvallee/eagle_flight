/*
 * File: discovery_stm32f7_driver.c
 *
 * Written by duvallee in 2018
 *
 */
#include "main.h"
#include "discovery_stm32f7_driver.h"
#if defined(NET_LWIP)
#include "lwip.h"
#endif

#if defined(FT5536)
#include "touch_ft5536.h"
#endif

#if (defined(P_NUCLEO_53L0A1) && defined(SENSOR_VL53L0X))
#include "vl53l0x_platform.h"
#include "vl53l0x_api.h"
#endif

#if defined(GPIO_STMPE1600)
#include "gpio_stmpe1600.h"
#endif

// --------------------------------------------------------------------------
static SDRAM_HandleTypeDef g_SDRAM_handle;
static LTDC_HandleTypeDef g_LTDC_handle;
static DMA2D_HandleTypeDef g_DMA2D_handle;

static I2C_HandleTypeDef g_I2C_Bus1_handle;
static I2C_HandleTypeDef g_I2C_Bus3_handle;

#if defined(TIMER_TEST)
static TIM_HandleTypeDef g_htim1_timer;
static TIM_HandleTypeDef g_htim2_timer;
static TIM_HandleTypeDef g_htim12_timer;
#endif

#define V530L0X_SINGLE_DELAY                             10
#define TOUCH_POLLING_DELAY                              10

#if defined(P_NUCLEO_53L0A1)
#define DEFAULT_53L0A1_I2C_ADDR                          0x52

#define VL53L0_A1_CENTER_PORT                            0
#define VL53L0_A1_LEFT_PORT                              1
#define VL53L0_A1_RIGHT_PORT                             2
#define VL53L0_A1_MAX_PORT                               3

static VL53L0X_Dev_t VL53L0XDevs[]                       =
{
   {
#if defined(GPIO_STMPE1600)
      .Id                                                = XNUCLEO53L1A1_DEV_CENTER,
#else
      .Id                                                = 0,
#endif   // GPIO_STMPE1600
      .DevLetter                                         =  'c',
      .I2cHandle                                         = &g_I2C_Bus1_handle,
      .I2cDevAddr                                        = DEFAULT_53L0A1_I2C_ADDR,
      .comms_speed_khz                                   = 400,
      .comms_type                                        = 1,
      .Present                                           = 0,
   },
   {
#if defined(GPIO_STMPE1600)
      .Id                                                = XNUCLEO53L1A1_DEV_LEFT,
#else
      .Id                                                = 1,
#endif   // GPIO_STMPE1600
      .DevLetter                                         = 'l',
      .I2cHandle                                         = &g_I2C_Bus1_handle,
      .I2cDevAddr                                        =DEFAULT_53L0A1_I2C_ADDR,
      .comms_speed_khz                                   = 400,
      .comms_type                                        = 1,
      .Present                                           = 0,
   },
   {
#if defined(GPIO_STMPE1600)
      .Id                                                = XNUCLEO53L1A1_DEV_RIGHT,
#else
      .Id                                                = 2,
#endif   // GPIO_STMPE1600
      .DevLetter                                         = 'r',
      .I2cHandle                                         = &g_I2C_Bus1_handle,
      .I2cDevAddr                                        = DEFAULT_53L0A1_I2C_ADDR,
      .comms_speed_khz                                   = 400,
      .comms_type                                        = 1,
      .Present                                           = 0,
   },
};

#if defined(STEMWIN)
typedef struct
{
   GUI_RECT frame_rect;
   I16 round;
   GUI_COLOR frame_color;
   GUI_RECT text_rect;   
} VL53L0_A1_GUI;

static VL53L0_A1_GUI g_vl5310_ar_gui[]                   =
{
   {
      { 169,  99, 310, 173},
      10,
      GUI_CYAN,
      { 179, 109, 300, 162},
   },
   {
      {   9,  99, 150, 173},
      10,
      GUI_CYAN,
      {  19, 109, 140, 162},
   },
   {
      { 329,  99, 470, 173},
      10,
      GUI_CYAN,
      { 339, 109, 460, 162},
   },
};
#endif   // STEMWIN
#endif   // P_NUCLEO_53L0A1

#if (defined(RTOS_FREERTOS) && defined(FT5536))
SemaphoreHandle_t g_touch_event_Semaphore                = NULL;
#endif

#if (defined(P_NUCLEO_53L0A1) && defined(RTOS_FREERTOS))
SemaphoreHandle_t g_i2c_bus_1_semaphore                  = NULL;

SemaphoreHandle_t g_53l0a1_semaphore                     = NULL;

SemaphoreHandle_t g_53l0a1_left_event_Semaphore          = NULL;
SemaphoreHandle_t g_53l0a1_center_event_Semaphore        = NULL;
SemaphoreHandle_t g_53l0a1_right_event_Semaphore         = NULL;
#endif



#if defined(TIMER_TEST)
/* --------------------------------------------------------------------------
 * Name : tim1_up_tim10_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void tim1_up_tim10_IRQHandler()
{
   HAL_TIM_IRQHandler(&g_htim1_timer);
   HAL_GPIO_TogglePin(ARDUINO_CN4_PIN_08_PORT, ARDUINO_CN4_PIN_08);
}

/* --------------------------------------------------------------------------
 * Name : test_timer1()
 *
 *
 * -------------------------------------------------------------------------- */
static void test_timer1()
{
   GPIO_InitTypeDef GPIO_InitStruct                      = {0, };
   TIM_ClockConfigTypeDef sClockSourceConfig             = {0, };
   TIM_MasterConfigTypeDef sMasterConfig                 = {0, };
   TIM_OC_InitTypeDef sConfigOC                          = {0, };
   TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig   = {0, };

   __HAL_RCC_GPIOI_CLK_ENABLE();
   __HAL_RCC_GPIOA_CLK_ENABLE();

   // for test
   GPIO_InitStruct.Pin                                   = ARDUINO_CN4_PIN_08;
   GPIO_InitStruct.Mode                                  = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull                                  = GPIO_NOPULL;
   GPIO_InitStruct.Speed                                 = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(ARDUINO_CN4_PIN_08_PORT, &GPIO_InitStruct);

   g_htim1_timer.Instance                                = TIM1;
   g_htim1_timer.Init.Prescaler                          = 3;                           // Freq 50MHz(200MHz / (3 + 1))
   g_htim1_timer.Init.CounterMode                        = TIM_COUNTERMODE_UP;
//   g_htim1_timer.Init.CounterMode                        = TIM_COUNTERMODE_CENTERALIGNED1;

   g_htim1_timer.Init.Period                             = 999;                         // 50 KHz (100MHz / 1000) : 20us

   g_htim1_timer.Init.ClockDivision                      = TIM_CLOCKDIVISION_DIV1;

   if (HAL_TIM_Base_Init(&g_htim1_timer) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   sClockSourceConfig.ClockSource                        = TIM_CLOCKSOURCE_INTERNAL;
   if (HAL_TIM_ConfigClockSource(&g_htim1_timer, &sClockSourceConfig) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   sMasterConfig.MasterOutputTrigger                     = TIM_TRGO_RESET;
   sMasterConfig.MasterSlaveMode                         = TIM_MASTERSLAVEMODE_DISABLE;
   HAL_TIMEx_MasterConfigSynchronization(&g_htim1_timer, &sMasterConfig);

   sConfigOC.OCMode                                      = TIM_OCMODE_TOGGLE;
   sConfigOC.Pulse                                       = 0;
   sConfigOC.OCPolarity                                  = TIM_OCPOLARITY_HIGH;
   sConfigOC.OCNPolarity                                 = TIM_OCNPOLARITY_HIGH;
   sConfigOC.OCFastMode                                  = TIM_OCFAST_DISABLE;
   sConfigOC.OCIdleState                                 = TIM_OCIDLESTATE_RESET;
   sConfigOC.OCNIdleState                                = TIM_OCNIDLESTATE_RESET;
   HAL_TIM_OC_ConfigChannel(&g_htim1_timer, &sConfigOC, TIM_CHANNEL_1);

   sBreakDeadTimeConfig.OffStateRunMode                  = TIM_OSSR_DISABLE;
   sBreakDeadTimeConfig.OffStateIDLEMode                 = TIM_OSSI_DISABLE;
   sBreakDeadTimeConfig.LockLevel                        = TIM_LOCKLEVEL_OFF;
   sBreakDeadTimeConfig.DeadTime                         = 0;
   sBreakDeadTimeConfig.BreakState                       = TIM_BREAK_DISABLE;
   sBreakDeadTimeConfig.BreakPolarity                    = TIM_BREAKPOLARITY_HIGH;
   sBreakDeadTimeConfig.BreakFilter                      = 0;
   sBreakDeadTimeConfig.Break2State                      = TIM_BREAK2_DISABLE;
   sBreakDeadTimeConfig.Break2Polarity                   = TIM_BREAK2POLARITY_HIGH;
   sBreakDeadTimeConfig.Break2Filter                     = 0;
   sBreakDeadTimeConfig.AutomaticOutput                  = TIM_AUTOMATICOUTPUT_DISABLE;
   if (HAL_TIMEx_ConfigBreakDeadTime(&g_htim1_timer, &sBreakDeadTimeConfig) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   // Set priority for interrupt
   HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 8, 0);
   // Enable USBHS Interrupt
   HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

   // enable update interrupt
   HAL_TIM_Base_Start_IT(&g_htim1_timer);

   // Output compare enable
   HAL_TIM_OC_Start(&g_htim1_timer, TIM_CHANNEL_1);
}

/* --------------------------------------------------------------------------
 * Name : test_timer2()
 *
 *
 * -------------------------------------------------------------------------- */
static void test_timer2()
{
   TIM_ClockConfigTypeDef sClockSourceConfig             = {0, };
   TIM_MasterConfigTypeDef sMasterConfig                 = {0, };
   TIM_OC_InitTypeDef sConfigOC                          = {0, };

   g_htim2_timer.Instance                                = TIM2;
   g_htim2_timer.Init.Prescaler                          = 9;                           // Freq 10MHz(100MHz / (1 + 9))
   g_htim2_timer.Init.CounterMode                        = TIM_COUNTERMODE_UP;

   g_htim2_timer.Init.Period                             = 9999;                        // 1 KHz (10MHz / 10000) : 1ms

   g_htim2_timer.Init.ClockDivision                      = TIM_CLOCKDIVISION_DIV1;

   if (HAL_TIM_Base_Init(&g_htim2_timer) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   sClockSourceConfig.ClockSource                        = TIM_CLOCKSOURCE_INTERNAL;
   if (HAL_TIM_ConfigClockSource(&g_htim2_timer, &sClockSourceConfig) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   sMasterConfig.MasterOutputTrigger                     = TIM_TRGO_RESET;
   sMasterConfig.MasterSlaveMode                         = TIM_MASTERSLAVEMODE_DISABLE;
   HAL_TIMEx_MasterConfigSynchronization(&g_htim2_timer, &sMasterConfig);

   sConfigOC.OCMode                                      = TIM_OCMODE_TOGGLE;
   sConfigOC.Pulse                                       = 0;
   sConfigOC.OCPolarity                                  = TIM_OCPOLARITY_HIGH;
   sConfigOC.OCNPolarity                                 = TIM_OCNPOLARITY_HIGH;
   sConfigOC.OCFastMode                                  = TIM_OCFAST_DISABLE;
   sConfigOC.OCIdleState                                 = TIM_OCIDLESTATE_RESET;
   sConfigOC.OCNIdleState                                = TIM_OCNIDLESTATE_RESET;
   HAL_TIM_OC_ConfigChannel(&g_htim2_timer, &sConfigOC, TIM_CHANNEL_1);

   // Output compare enable
   HAL_TIM_OC_Start(&g_htim2_timer, TIM_CHANNEL_1);
}


/* --------------------------------------------------------------------------
 * Name : test_timer12()
 *
 *
 * -------------------------------------------------------------------------- */
static void test_timer12()
{
   TIM_ClockConfigTypeDef sClockSourceConfig             = {0, };
   TIM_MasterConfigTypeDef sMasterConfig                 = {0, };
   TIM_OC_InitTypeDef sConfigOC                          = {0, };

   g_htim12_timer.Instance                               = TIM12;
   g_htim12_timer.Init.Prescaler                         = 0;                           // Freq 100MHz(100MHz / (1 + 0))
   g_htim12_timer.Init.CounterMode                       = TIM_COUNTERMODE_UP;

   g_htim12_timer.Init.Period                            = 999;                         // 100 KHz (100MHz / 1000) : 10us

   g_htim12_timer.Init.ClockDivision                     = TIM_CLOCKDIVISION_DIV1;

   if (HAL_TIM_Base_Init(&g_htim12_timer) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   sClockSourceConfig.ClockSource                        = TIM_CLOCKSOURCE_INTERNAL;
   if (HAL_TIM_ConfigClockSource(&g_htim12_timer, &sClockSourceConfig) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   sMasterConfig.MasterOutputTrigger                     = TIM_TRGO_RESET;
   sMasterConfig.MasterSlaveMode                         = TIM_MASTERSLAVEMODE_DISABLE;
   HAL_TIMEx_MasterConfigSynchronization(&g_htim12_timer, &sMasterConfig);

   sConfigOC.OCMode                                      = TIM_OCMODE_PWM1;
   sConfigOC.Pulse                                       = 245;
   sConfigOC.OCPolarity                                  = TIM_OCPOLARITY_HIGH;
   sConfigOC.OCNPolarity                                 = TIM_OCNPOLARITY_HIGH;
   sConfigOC.OCFastMode                                  = TIM_OCFAST_DISABLE;
   sConfigOC.OCIdleState                                 = TIM_OCIDLESTATE_RESET;
   sConfigOC.OCNIdleState                                = TIM_OCNIDLESTATE_RESET;
   HAL_TIM_PWM_ConfigChannel(&g_htim12_timer, &sConfigOC, TIM_CHANNEL_1);

   sConfigOC.OCMode                                      = TIM_OCMODE_PWM2;
   sConfigOC.Pulse                                       = 245;
   sConfigOC.OCPolarity                                  = TIM_OCPOLARITY_HIGH;
   sConfigOC.OCNPolarity                                 = TIM_OCNPOLARITY_HIGH;
   sConfigOC.OCFastMode                                  = TIM_OCFAST_DISABLE;
   sConfigOC.OCIdleState                                 = TIM_OCIDLESTATE_RESET;
   sConfigOC.OCNIdleState                                = TIM_OCNIDLESTATE_RESET;
   HAL_TIM_PWM_ConfigChannel(&g_htim12_timer, &sConfigOC, TIM_CHANNEL_2);

   // PWM enable
   HAL_TIM_PWM_Start(&g_htim12_timer, TIM_CHANNEL_1);
   HAL_TIM_PWM_Start(&g_htim12_timer, TIM_CHANNEL_2);

}

#endif


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
 * Name : BSP_LTDC_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void BSP_LTDC_IRQHandler(void)
{
   HAL_LTDC_IRQHandler(&g_LTDC_handle);
}


/* --------------------------------------------------------------------------
 * Name : BSP_LTDC_Init()
 *        LTDC init function
 *
 * -------------------------------------------------------------------------- */
static void BSP_LTDC_Init(void)
{
   LTDC_LayerCfgTypeDef pLayerCfg;

   g_LTDC_handle.Instance                                = LTDC;

   // Polarity
   g_LTDC_handle.Init.HSPolarity                         = LTDC_HSPOLARITY_AL;
   g_LTDC_handle.Init.VSPolarity                         = LTDC_VSPOLARITY_AL;
   g_LTDC_handle.Init.DEPolarity                         = LTDC_DEPOLARITY_AL;
   g_LTDC_handle.Init.PCPolarity                         = LTDC_PCPOLARITY_IPC;

   // timing
   g_LTDC_handle.Init.HorizontalSync                     = (RK043FN48H_HSYNC - 1);
   g_LTDC_handle.Init.VerticalSync                       = (RK043FN48H_VSYNC - 1);
   g_LTDC_handle.Init.AccumulatedHBP                     = (RK043FN48H_HSYNC + RK043FN48H_HBP - 1);;
   g_LTDC_handle.Init.AccumulatedVBP                     = (RK043FN48H_VSYNC + RK043FN48H_VBP- 1);;
   g_LTDC_handle.Init.AccumulatedActiveW                 = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
   g_LTDC_handle.Init.AccumulatedActiveH                 = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
   g_LTDC_handle.Init.TotalWidth                         = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP + RK043FN48H_HFP - 1);
   g_LTDC_handle.Init.TotalHeigh                         = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP + RK043FN48H_VFP - 1);;

   // background value
   g_LTDC_handle.Init.Backcolor.Blue                     = 0;
   g_LTDC_handle.Init.Backcolor.Green                    = 0;
   g_LTDC_handle.Init.Backcolor.Red                      = 0;

   if (HAL_LTDC_Init(&g_LTDC_handle) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   HAL_LTDC_ProgramLineEvent(&g_LTDC_handle, 0);
   // Enable dithering
   HAL_LTDC_EnableDither(&g_LTDC_handle);

   pLayerCfg.WindowX0                                    = 0;
   pLayerCfg.WindowX1                                    = (RK043FN48H_WIDTH - 1);
   pLayerCfg.WindowY0                                    = 0;
   pLayerCfg.WindowY1                                    = (RK043FN48H_HEIGHT - 1);
   pLayerCfg.PixelFormat                                 = LTDC_PIXEL_FORMAT_ARGB8888;
   pLayerCfg.Alpha                                       = 255;
   pLayerCfg.Alpha0                                      = 0;
   pLayerCfg.BlendingFactor1                             = LTDC_BLENDING_FACTOR1_CA;
   pLayerCfg.BlendingFactor2                             = LTDC_BLENDING_FACTOR2_CA;
   pLayerCfg.FBStartAdress                               = (uint32_t) LCD_FRAMEBUFFER_LAYER_0;
   pLayerCfg.ImageWidth                                  = RK043FN48H_WIDTH;
   pLayerCfg.ImageHeight                                 = RK043FN48H_HEIGHT;
   pLayerCfg.Backcolor.Blue                              = 0;
   pLayerCfg.Backcolor.Green                             = 0;
   pLayerCfg.Backcolor.Red                               = 0;
   if (HAL_LTDC_ConfigLayer(&g_LTDC_handle, &pLayerCfg, 0) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }
}

/* --------------------------------------------------------------------------
 * Name : MX_DMA2D_Init()
 *
 *
 * -------------------------------------------------------------------------- */
static void BSP_DMA2D_Init(void)
{
   g_DMA2D_handle.Instance                               = DMA2D;
   g_DMA2D_handle.Init.Mode                              = DMA2D_M2M;
   g_DMA2D_handle.Init.ColorMode                         = DMA2D_OUTPUT_ARGB8888;
   g_DMA2D_handle.Init.OutputOffset                      = 0;
   g_DMA2D_handle.LayerCfg[1].InputOffset                = 0;
   g_DMA2D_handle.LayerCfg[1].InputColorMode             = DMA2D_INPUT_ARGB8888;
   g_DMA2D_handle.LayerCfg[1].AlphaMode                  = DMA2D_NO_MODIF_ALPHA;
   g_DMA2D_handle.LayerCfg[1].InputAlpha                 = 0;
   if (HAL_DMA2D_Init(&g_DMA2D_handle) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   if (HAL_DMA2D_ConfigLayer(&g_DMA2D_handle, 1) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }
}

/* --------------------------------------------------------------------------
 * Name : BSP_I2C_BUS1_Init()
 *
 *
 * -------------------------------------------------------------------------- */
static void BSP_I2C_BUS1_Init(void)
{
#if defined(P_NUCLEO_53L0A1)
//   BSP_I2C_BUS1_FailRecover();
#endif

   g_I2C_Bus1_handle.Instance                            = I2C1;
#if (defined(USE_HAL_DRIVER) && defined(STM32F4XX))
   g_I2C_Bus1_handle.Init.ClockSpeed                     = 400000;                                 // fast I2C
   g_I2C_Bus1_handle.Init.DutyCycle                      = I2C_DUTYCYCLE_2;
#else
   g_I2C_Bus1_handle.Init.Timing                         = 0x00303D5B;
#endif
   g_I2C_Bus1_handle.Init.OwnAddress1                    = 0;
   g_I2C_Bus1_handle.Init.AddressingMode                 = I2C_ADDRESSINGMODE_7BIT;
   g_I2C_Bus1_handle.Init.DualAddressMode                = I2C_DUALADDRESS_DISABLE;
   g_I2C_Bus1_handle.Init.OwnAddress2                    = 0;
   g_I2C_Bus1_handle.Init.OwnAddress2Masks               = I2C_OA2_NOMASK;
   g_I2C_Bus1_handle.Init.GeneralCallMode                = I2C_GENERALCALL_DISABLE;
   g_I2C_Bus1_handle.Init.NoStretchMode                  = I2C_NOSTRETCH_DISABLE;
   if (HAL_I2C_Init(&g_I2C_Bus1_handle) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
      return;
   }

   // Configure Analogue filter 
   if (HAL_I2CEx_ConfigAnalogFilter(&g_I2C_Bus1_handle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
      return;
   }

   // Configure Digital filter 
   if (HAL_I2CEx_ConfigDigitalFilter(&g_I2C_Bus1_handle, 0) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
      return;
   }
}

/* --------------------------------------------------------------------------
 * Name : BSP_I2C_BUS1_Read()
 *
 *
 * -------------------------------------------------------------------------- */
int BSP_I2C_BUS1_Read(uint16_t device_addr, uint16_t reg_addr, uint16_t reg_addr_size, uint8_t* pData, uint16_t DataSize)
{
   HAL_StatusTypeDef status                              = HAL_OK;
   status                                                = HAL_I2C_Mem_Read(&g_I2C_Bus1_handle, device_addr, (uint16_t) reg_addr, reg_addr_size, pData, DataSize, 1000);

   // Check the communication status
   if (status != HAL_OK)
   {
      debug_output_error("HAL_I2C_Mem_Read Failed() : %d \r\n", status);
      return -1;
   }
   return 0;
}

/* --------------------------------------------------------------------------
 * Name : BSP_I2C_BUS1_Write()
 *
 *
 * -------------------------------------------------------------------------- */
int BSP_I2C_BUS1_Write(uint16_t device_addr, uint16_t reg_addr, uint16_t reg_addr_size, uint8_t* pData, uint16_t DataSize)
{
   HAL_StatusTypeDef status                              = HAL_OK;
   status                                                = HAL_I2C_Mem_Write(&g_I2C_Bus1_handle, device_addr, (uint16_t) reg_addr, reg_addr_size, pData, DataSize, 1000);

   // Check the communication status
   if (status != HAL_OK)
   {
      debug_output_error("HAL_I2C_Mem_Read Failed() : %d \r\n", status);
      return -1;
   }
   return 0;
}

/* --------------------------------------------------------------------------
 * Name : BSP_I2C_BUS3_Init()
 *
 *
 * -------------------------------------------------------------------------- */
static void BSP_I2C_BUS3_Init(void)
{
   g_I2C_Bus3_handle.Instance                            = I2C3;
#if (defined(USE_HAL_DRIVER) && defined(STM32F4XX))
   g_I2C_Bus3_handle.Init.ClockSpeed                     = 400000;                                 // fast I2C
   g_I2C_Bus3_handle.Init.DutyCycle                      = I2C_DUTYCYCLE_2;
#else
   g_I2C_Bus3_handle.Init.Timing                         = 0x00303D5B;
#endif
   g_I2C_Bus3_handle.Init.OwnAddress1                    = 0;
   g_I2C_Bus3_handle.Init.AddressingMode                 = I2C_ADDRESSINGMODE_7BIT;
   g_I2C_Bus3_handle.Init.DualAddressMode                = I2C_DUALADDRESS_DISABLE;
   g_I2C_Bus3_handle.Init.OwnAddress2                    = 0;
   g_I2C_Bus3_handle.Init.OwnAddress2Masks               = I2C_OA2_NOMASK;
   g_I2C_Bus3_handle.Init.GeneralCallMode                = I2C_GENERALCALL_DISABLE;
   g_I2C_Bus3_handle.Init.NoStretchMode                  = I2C_NOSTRETCH_DISABLE;
   if (HAL_I2C_Init(&g_I2C_Bus3_handle) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
      return;
   }

   // Configure Analogue filter 
   if (HAL_I2CEx_ConfigAnalogFilter(&g_I2C_Bus3_handle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
      return;
   }

   // Configure Digital filter 
   if (HAL_I2CEx_ConfigDigitalFilter(&g_I2C_Bus3_handle, 0) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
      return;
   }
}

#if defined(FT5536)
/* --------------------------------------------------------------------------
 * Name : BSP_TOUCH_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void BSP_TOUCH_IRQHandler(void)
{
#if defined(RTOS_FREERTOS)
   BaseType_t xHigherPriorityTaskWoken                   = pdFALSE;
   if (g_touch_event_Semaphore != NULL)
   {
      // xEventGroupSetBitsFromISR
      // xEventGroupWaitBits
      xSemaphoreGiveFromISR(g_touch_event_Semaphore, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
   }
#endif   // RTOS_FREERTOS

//   debug_output_info("Touch interrupt : %d \r\n", HAL_GPIO_ReadPin(TOUCH_FT5536_INT_PORT, TOUCH_FT5536_INT_PIN));
}

#if defined(RTOS_FREERTOS)
/* --------------------------------------------------------------------------
 * Name : touch_event_task()
 *
 *
 * -------------------------------------------------------------------------- */
void touch_event_task(void const* argument)
{
#if defined(STEMWIN)
   TOUCH_EVENT_STRUCT touch_event;
#endif
   int i                                                 = 0;
   while (1)
   {
      if (xSemaphoreTake(g_touch_event_Semaphore, 1000) == pdPASS)
      {
         touch_start(TOUCH_FT5536_TRIGGER_MODE_POLLING);
         while(1)
         {
            if (get_ft5536_event(&touch_event) > 0)
            {
#if defined(STEMWIN)
               GUI_CURSOR_SetPosition(touch_event.touch_point[0].x_pos, touch_event.touch_point[0].y_pos);
               GUI_CURSOR_Show();
#endif
//               debug_output_dump("pt=%d, ", touch_event.touch_point_num);

               for (i = 0; i < touch_event.touch_point_num; i++)
               {
//                  debug_output_dump("[x=%3d, y=%3d, ", touch_event.touch_point[i].x_pos, touch_event.touch_point[i].y_pos);
//                  debug_output_dump("action=%d,", touch_event.touch_point[i].touch_action);
//                  debug_output_dump("weight=%3d, ", touch_event.touch_point[i].touch_weight);
//                  debug_output_dump("misc=%d] ", touch_event.touch_point[i].touch_area);
               }
            }
            else
            {
               touch_start(TOUCH_FT5536_TRIGGER_MODE_INTERRUPT);
               break;
            }
//               debug_output_dump("%c\r\n", ' ');
            osDelay(TOUCH_POLLING_DELAY);
         }
      }
      else
      {
         touch_start(TOUCH_FT5536_TRIGGER_MODE_INTERRUPT);
      }
   }
}
#endif      // RTOS_FREERTOS
#endif      // FT5536


#if defined(P_NUCLEO_53L0A1)
/* --------------------------------------------------------------------------
 * Name : Nucleo_53l0a1_init()
 *
 *
 * -------------------------------------------------------------------------- */
int Nucleo_53l0a1_init()
{
   VL53L0X_Version_t vl53l0x_version;
   int status                                            = 0;
   int i                                                 = 0;
   uint16_t Id                                           = 0;
   int FinalAddress                                      = 0;

   // -------------------------------------------------------------------------------------------------------
   // check sensor and change I2C Address
   // Detect Sensor of V53L0A1
   gpio_stmpe1600_reset(XNUCLEO53L1A1_DEV_LEFT, 0);
   gpio_stmpe1600_reset(XNUCLEO53L1A1_DEV_CENTER, 0);
   gpio_stmpe1600_reset(XNUCLEO53L1A1_DEV_RIGHT, 0);

   if (VL53L0X_GetVersion(&vl53l0x_version)  != VL53L0X_ERROR_NONE)
   {
      debug_output_error("VL53L0X_GetVersion() failed !!! \r\n");
      return -1;
   }
   debug_output_info("------------------------------------------------------ \r\n");
   debug_output_info("X-CUBE-53L0A1 : Ver %d.%d.%d Rev. %d \r\n", (int) vl53l0x_version.major, (int) vl53l0x_version.minor, (int) vl53l0x_version.build, (int) vl53l0x_version.revision);
   debug_output_info("------------------------------------------------------ \r\n");

   for (i = 0; i < VL53L0_A1_MAX_PORT; i++)
   {
      VL53L0X_Dev_t *pDev                                = &VL53L0XDevs[i];
      pDev->I2cDevAddr                                   = DEFAULT_53L0A1_I2C_ADDR;
      pDev->Present                                      = 0;
      gpio_stmpe1600_reset(pDev->Id, 1);
      HAL_Delay(2);
      FinalAddress                                       = DEFAULT_53L0A1_I2C_ADDR + (i + 1) * 2;
      do
      {
         // Set I2C standard mode (400 KHz) before doing the first register access
         status                                          = VL53L0X_WrByte(pDev, 0x88, 0x00);

         // Try to read one register using default 0x52 address
         status                                          = VL53L0X_RdWord(pDev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
         if (status)
         {
            debug_output_error("#%d Read id failed !!! \r\n", i);
            break;
         }

         if (Id == 0xEEAA)
         {
            // Sensor is found => Change its I2C address to final one
            status                                       = VL53L0X_SetDeviceAddress(pDev, FinalAddress);
            if (status != 0)
            {
               debug_output_error("#%d VL53L0X_SetDeviceAddress (0x%x) failed \r\n", i, FinalAddress);
               break;
            }
            pDev->I2cDevAddr                             = FinalAddress;

            // Check all is OK with the new I2C address and initialize the sensor
            status                                       = VL53L0X_RdWord(pDev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
            if (status != 0)
            {
               debug_output_error("#%d VL53L0X_RdWord failed by changed i2c address \r\n", i);
               break;
            }

            status                                       = VL53L0X_DataInit(pDev);
            if (status == 0)
            {
               pDev->Present                             = 1;
            }
            else
            {
               debug_output_error("VL53L0X_DataInit %d fail \r\n", i);
               break;
            }
            debug_output_info("VL53L0X %d Present and initiated to final 0x%x \r\n", pDev->Id, pDev->I2cDevAddr);
//            nDevPresent++;
//            nDevMask                                     |= 1 << i;
         }
         else
         {
            debug_output_error("#%d unknown ID %x\n", i, Id);
            status                                       = 1;
         }
      } while (0);
      // if fail r can't use for any reason then put the  device back to reset
      if (status)
      {
         gpio_stmpe1600_reset(pDev->Id, 0);
         pDev->Present                                   = 0;
      }
   }

   return 0;
}

/* --------------------------------------------------------------------------
 * Name : vl53l0x_Single_Mode_init()
 *
 *
 * -------------------------------------------------------------------------- */
static int vl53l0x_Single_Mode_init(VL53L0X_Dev_t *pDev)
{
   uint8_t VhvSettings;
   uint8_t PhaseCal;
   uint32_t refSpadCount;
   uint8_t isApertureSpads;

   FixPoint1616_t signalLimit                            = (FixPoint1616_t) (0.25 * 65536);
   FixPoint1616_t sigmaLimit                             = (FixPoint1616_t) (18 * 65536);
   uint32_t timingBudget                                 = 33000;
   uint8_t preRangeVcselPeriod                           = 14;
   uint8_t finalRangeVcselPeriod                         = 10;

   if (pDev->Present == 0)
   {
      debug_output_warn("Does not exist : #%d. \r\n", pDev->Id);
      return -1;
   }
   if (VL53L0X_StaticInit(pDev) != VL53L0X_ERROR_NONE)
   {
      debug_output_error("VL53L0X_StaticInit() failed \r\n");
      return -1;
   }

   // Perform a reference calibarion of the Device. This function should be run from time to time before dong a ranging measurement.
   if (VL53L0X_PerformRefCalibration(pDev, &VhvSettings, &PhaseCal) != VL53L0X_ERROR_NONE)
   {
      debug_output_warn("VL53L0X_PerformRefCalibration() failed : #%d. \r\n", pDev->Id);
   }

   // the reference SPAD initialization procedure determines the minimum amount of reference spads to be enables to archieve a target
   // reference signal rate and sould be performaed once during initialization.
   if (VL53L0X_PerformRefSpadManagement(pDev, &refSpadCount, &isApertureSpads) != VL53L0X_ERROR_NONE)
   {
      debug_output_warn("VL53L0X_PerformRefSpadManagement() failed : #%d. \r\n", pDev->Id);
   }

   // Set device to a new mode
   if (VL53L0X_SetDeviceMode(pDev, VL53L0X_DEVICEMODE_SINGLE_RANGING) != VL53L0X_ERROR_NONE)
   {
      debug_output_error("VL53L0X_SetDeviceMode() - VL53L0X_DEVICEMODE_SINGLE_RANGING failed \r\n");
      return -1;
   }

   // This function Enable/Disable a specific limit check. The limit check is identified with the LimitCheckId.
   if (VL53L0X_SetLimitCheckEnable(pDev, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1) != VL53L0X_ERROR_NONE)
   {
      debug_output_warn("VL53L0X_SetLimitCheckEnable() failed : #%d. \r\n", pDev->Id);
   }

   if (VL53L0X_SetLimitCheckEnable(pDev, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1) != VL53L0X_ERROR_NONE)
   {
      debug_output_warn("VL53L0X_SetLimitCheckEnable() failed : #%d. \r\n", pDev->Id);
   }

   switch(pDev->single_shot_option)
   {
      case VL53L0X_RUNNING_SINGLE_SHOT_LONG_RANGE :
         signalLimit                                     = (FixPoint1616_t) (0.1 * 65536);
         sigmaLimit                                      = (FixPoint1616_t) (60 * 65536);
         timingBudget                                    = 33000;
         preRangeVcselPeriod                             = 18;
         finalRangeVcselPeriod                           = 14;
         break;

      case VL53L0X_RUNNING_SINGLE_SHOT_HIGH_ACCURACY :
         signalLimit                                     = (FixPoint1616_t) (0.25 * 65536);
         sigmaLimit                                      = (FixPoint1616_t) (18 * 65536);
         timingBudget                                    = 200000;
         preRangeVcselPeriod                             = 14;
         finalRangeVcselPeriod                           = 10;
         break;

      case VL53L0X_RUNNING_SINGLE_SHOT_HIGH_SPEED :
         signalLimit                                     = (FixPoint1616_t) (0.25 * 65536);
         sigmaLimit                                      = (FixPoint1616_t) (32 * 65536);
         timingBudget                                    = 20000;
         preRangeVcselPeriod                             = 14;
         finalRangeVcselPeriod                           = 10;
         break;
   }

   if (VL53L0X_SetLimitCheckValue(pDev,  VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, signalLimit) != VL53L0X_ERROR_NONE)
   {
      debug_output_warn("VL53L0X_SetLimitCheckValue() failed : #%d. \r\n", pDev->Id);
   }

   if (VL53L0X_SetLimitCheckValue(pDev,  VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, sigmaLimit) != VL53L0X_ERROR_NONE)
   {
      debug_output_warn("VL53L0X_SetLimitCheckValue() failed : #%d. \r\n", pDev->Id);
   }

   if (VL53L0X_SetMeasurementTimingBudgetMicroSeconds(pDev,  timingBudget) != VL53L0X_ERROR_NONE)
   {
      debug_output_warn("VL53L0X_SetMeasurementTimingBudgetMicroSeconds() failed : #%d. \r\n", pDev->Id);
   }
   
   if (VL53L0X_SetVcselPulsePeriod(pDev,  VL53L0X_VCSEL_PERIOD_PRE_RANGE, preRangeVcselPeriod) != VL53L0X_ERROR_NONE)
   {
      debug_output_warn("VL53L0X_SetVcselPulsePeriod() failed : #%d. \r\n", pDev->Id);
   }

   if (VL53L0X_SetVcselPulsePeriod(pDev,  VL53L0X_VCSEL_PERIOD_FINAL_RANGE, finalRangeVcselPeriod) != VL53L0X_ERROR_NONE)
   {
      debug_output_warn("VL53L0X_SetVcselPulsePeriod() failed : #%d. \r\n", pDev->Id);
   }

   if (VL53L0X_PerformRefCalibration(pDev, &VhvSettings, &PhaseCal) != VL53L0X_ERROR_NONE)
   {
      debug_output_warn("VL53L0X_SetVcselPulsePeriod() failed : #%d. \r\n", pDev->Id);
   }

   return 0;
}

/* --------------------------------------------------------------------------
 * Name : vl53l0x_Continuous_Mode_init()
 *
 *
 * -------------------------------------------------------------------------- */
static int vl53l0x_Continuous_Mode_init(VL53L0X_Dev_t *pDev)
{
   uint8_t VhvSettings;
   uint8_t PhaseCal;
   uint32_t refSpadCount;
   uint8_t isApertureSpads;

   VL53L0X_StaticInit(pDev);
   VL53L0X_PerformRefCalibration(pDev, &VhvSettings, &PhaseCal);
   VL53L0X_PerformRefSpadManagement(pDev, &refSpadCount, &isApertureSpads);
   VL53L0X_SetInterMeasurementPeriodMilliSeconds(pDev, 250);
   VL53L0X_SetDeviceMode(pDev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
   return 0;
}

/* --------------------------------------------------------------------------
 * Name : BSP_53L0A1_LEFT_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void BSP_53L0A1_LEFT_IRQHandler(void)
{
#if defined(RTOS_FREERTOS)
   BaseType_t xHigherPriorityTaskWoken                   = pdFALSE;
   if (g_53l0a1_left_event_Semaphore != NULL)
   {
      xSemaphoreGiveFromISR(g_53l0a1_left_event_Semaphore, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
   }
//   debug_output_info("V53L0X-A0 irq \r\n");
#endif
}

/* --------------------------------------------------------------------------
 * Name : BSP_53L0A1_CENTER_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void BSP_53L0A1_CENTER_IRQHandler(void)
{
#if defined(RTOS_FREERTOS)
   BaseType_t xHigherPriorityTaskWoken                   = pdFALSE;
   if (g_53l0a1_center_event_Semaphore != NULL)
   {
      xSemaphoreGiveFromISR(g_53l0a1_center_event_Semaphore, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
   }
//   debug_output_info("V53L0X-A0 irq \r\n");
#endif
}

/* --------------------------------------------------------------------------
 * Name : BSP_53L0A1_RIGHT_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void BSP_53L0A1_RIGHT_IRQHandler(void)
{
#if defined(RTOS_FREERTOS)
   BaseType_t xHigherPriorityTaskWoken                   = pdFALSE;
   if (g_53l0a1_right_event_Semaphore != NULL)
   {
      xSemaphoreGiveFromISR(g_53l0a1_right_event_Semaphore, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
   }
//   debug_output_info("V53L0X-A0 irq \r\n");
#endif
}

#if defined(RTOS_FREERTOS)
int LeakyFactorFix8                                      = (int) (0.6 * 256);
/* --------------------------------------------------------------------------
 * Name : v53l0a1_center_event_task()
 *
 *
 * -------------------------------------------------------------------------- */
void v53l0a1_center_event_task(void const* argument)
{
   int status;
   VL53L0X_RangingMeasurementData_t RangingMeasurementData;
   VL53L0X_Dev_t* pDev                                   = &VL53L0XDevs[VL53L0_A1_CENTER_PORT];
#if defined(STEMWIN)
   VL53L0_A1_GUI* pGUI                                   = &g_vl5310_ar_gui[VL53L0_A1_CENTER_PORT];
static char measure_str[32];
   GUI_Clear();
#endif
   while (1)
   {
      if (pDev->running_mode == VL53L0X_RUNNING_SINGLE_SHOT_MODE)
      {
         xSemaphoreTake(g_53l0a1_semaphore, portMAX_DELAY);
         status                                             = VL53L0X_PerformSingleRangingMeasurement(pDev, &RangingMeasurementData);
         xSemaphoreGive(g_53l0a1_semaphore);

#if defined(STEMWIN)
         GUI_ClearRectEx(&(pGUI->text_rect));
         GUI_SetColor(pGUI->frame_color);
         GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
         GUI_SetPenSize(3);
         GUI_SetFont(&GUI_Font10_1);
         GUI_DrawRoundedRect(pGUI->frame_rect.x0, pGUI->frame_rect.y0, pGUI->frame_rect.x1, pGUI->frame_rect.y1, pGUI->round);
#endif

         if (status != VL53L0X_ERROR_NONE)
         {
            debug_output_warn("VL53L0X_PerformSingleRangingMeasurement() failed : Sensor of Center \r\n");
            osDelay(1000);
            continue;
         }
//         debug_output_info("%d : %d mm (status = %d, SignalRate = %d) \r\n", (int) pDev->Id, (int) RangingMeasurementData.RangeMilliMeter, RangingMeasurementData.RangeStatus, (int) RangingMeasurementData.SignalRateRtnMegaCps);
         if (RangingMeasurementData.RangeStatus == 0)
         {
            if (pDev->LeakyFirst)
            {
               pDev->LeakyFirst                          = 0;
               pDev->LeakyRange                          = RangingMeasurementData.RangeDMaxMilliMeter;
            }
            else
            {
               pDev->LeakyRange                          = (pDev->LeakyRange * LeakyFactorFix8 + (256 - LeakyFactorFix8) * RangingMeasurementData.RangeDMaxMilliMeter) >> 8;
            }
#if defined(STEMWIN)
            sprintf(measure_str, "%d mm", RangingMeasurementData.RangeMilliMeter);
            GUI__DispStringInRect(measure_str, &pGUI->text_rect, GUI_TA_CENTER | GUI_TA_VCENTER, strlen(measure_str));
#endif
         }
         else
         {
#if defined(STEMWIN)
            sprintf(measure_str, "~~~ mm");
            GUI__DispStringInRect(measure_str, &pGUI->text_rect, GUI_TA_CENTER | GUI_TA_VCENTER, strlen(measure_str));
#endif
            pDev->LeakyFirst                             = 1;
         }
         osDelay(V530L0X_SINGLE_DELAY);
      }
      else if (pDev->running_mode == VL53L0X_RUNNING_CONTINUOUS_MODE)
      {
         if (xSemaphoreTake(g_53l0a1_center_event_Semaphore, 1000) == pdPASS)
         {
         }
         else
         {
            VL53L0X_StopMeasurement(pDev);                           // it is safer to do this while sensor is stopped
            VL53L0X_ClearInterruptMask(pDev, -1);

            // Start continuous ranging
            VL53L0X_StartMeasurement(pDev);
            debug_output_info("... \r\n");
         }
      }
      else
      {
         osDelay(1000);
      }
   }
}


/* --------------------------------------------------------------------------
 * Name : v53l0a1_left_event_task()
 *
 *
 * -------------------------------------------------------------------------- */
void v53l0a1_left_event_task(void const* argument)
{
   int status;
   VL53L0X_RangingMeasurementData_t RangingMeasurementData;
   VL53L0X_Dev_t* pDev                                   = &VL53L0XDevs[VL53L0_A1_LEFT_PORT];
#if defined(STEMWIN)
   VL53L0_A1_GUI* pGUI                                   = &g_vl5310_ar_gui[VL53L0_A1_LEFT_PORT];
static char measure_str[32];
#endif
   while (1)
   {
      if (pDev->running_mode == VL53L0X_RUNNING_SINGLE_SHOT_MODE)
      {
         xSemaphoreTake(g_53l0a1_semaphore, portMAX_DELAY);
         status                                          = VL53L0X_PerformSingleRangingMeasurement(pDev, &RangingMeasurementData);
         xSemaphoreGive(g_53l0a1_semaphore);
#if defined(STEMWIN)
         GUI_ClearRectEx(&(pGUI->text_rect));
         GUI_SetColor(pGUI->frame_color);
         GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
         GUI_SetPenSize(3);
         GUI_SetFont(&GUI_Font10_1);
         GUI_DrawRoundedRect(pGUI->frame_rect.x0, pGUI->frame_rect.y0, pGUI->frame_rect.x1, pGUI->frame_rect.y1, pGUI->round);
#endif

         if (status != VL53L0X_ERROR_NONE)
         {
            debug_output_warn("VL53L0X_PerformSingleRangingMeasurement() failed : Sensor of Left \r\n");
            osDelay(1000);
            continue;
         }
//         debug_output_info("%d : %d mm (status = %d, SignalRate = %d) \r\n", (int) pDev->Id, (int) RangingMeasurementData.RangeMilliMeter, RangingMeasurementData.RangeStatus, (int) RangingMeasurementData.SignalRateRtnMegaCps);
         if (RangingMeasurementData.RangeStatus == 0)
         {
            if (pDev->LeakyFirst)
            {
               pDev->LeakyFirst = 0;
               pDev->LeakyRange = RangingMeasurementData.RangeDMaxMilliMeter;
            }
            else
            {
               pDev->LeakyRange = (pDev->LeakyRange * LeakyFactorFix8 +
                                                             (256 - LeakyFactorFix8) * RangingMeasurementData.RangeDMaxMilliMeter) >> 8;
            }
#if defined(STEMWIN)
            sprintf(measure_str, "%d mm", RangingMeasurementData.RangeMilliMeter);
            GUI__DispStringInRect(measure_str, &pGUI->text_rect, GUI_TA_CENTER | GUI_TA_VCENTER, strlen(measure_str));
#endif
         }
         else
         {
#if defined(STEMWIN)
            sprintf(measure_str, "~~~ mm");
            GUI__DispStringInRect(measure_str, &pGUI->text_rect, GUI_TA_CENTER | GUI_TA_VCENTER, strlen(measure_str));
#endif
            pDev->LeakyFirst = 1;
         }
         osDelay(V530L0X_SINGLE_DELAY);
      }
      else if (pDev->running_mode == VL53L0X_RUNNING_CONTINUOUS_MODE)
      {
         if (xSemaphoreTake(g_53l0a1_left_event_Semaphore, 1000) == pdPASS)
         {
         }
         else
         {
            VL53L0X_StopMeasurement(pDev);           // it is safer to do this while sensor is stopped
            VL53L0X_ClearInterruptMask(pDev, -1);

            // Start continuous ranging
            VL53L0X_StartMeasurement(pDev);
            debug_output_info("... \r\n");
         }
      }
      else
      {
         osDelay(1000);
      }
   }
}

/* --------------------------------------------------------------------------
 * Name : v53l0a1_right_event_task()
 *
 *
 * -------------------------------------------------------------------------- */
void v53l0a1_right_event_task(void const* argument)
{
   int status;
   VL53L0X_RangingMeasurementData_t RangingMeasurementData;
   VL53L0X_Dev_t* pDev                                   = &VL53L0XDevs[VL53L0_A1_RIGHT_PORT];
#if defined(STEMWIN)
   VL53L0_A1_GUI* pGUI                                   = &g_vl5310_ar_gui[VL53L0_A1_RIGHT_PORT];
static char measure_str[32];
#endif
   while (1)
   {
      if (pDev->running_mode == VL53L0X_RUNNING_SINGLE_SHOT_MODE)
      {
         xSemaphoreTake(g_53l0a1_semaphore, portMAX_DELAY);
         status                                          = VL53L0X_PerformSingleRangingMeasurement(pDev, &RangingMeasurementData);
         xSemaphoreGive(g_53l0a1_semaphore);
#if defined(STEMWIN)
         GUI_ClearRectEx(&(pGUI->text_rect));
         GUI_SetColor(pGUI->frame_color);
         GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
         GUI_SetPenSize(3);
         GUI_SetFont(&GUI_Font10_1);
         GUI_DrawRoundedRect(pGUI->frame_rect.x0, pGUI->frame_rect.y0, pGUI->frame_rect.x1, pGUI->frame_rect.y1, pGUI->round);
#endif
         if (status != VL53L0X_ERROR_NONE)
         {
            debug_output_warn("VL53L0X_PerformSingleRangingMeasurement() failed : Sensor of Center \r\n");
            osDelay(1000);
            continue;
         }
//         debug_output_info("%d : %d mm (status = %d, SignalRate = %d) \r\n", (int) pDev->Id, (int) RangingMeasurementData.RangeMilliMeter, RangingMeasurementData.RangeStatus, (int) RangingMeasurementData.SignalRateRtnMegaCps);
         if (RangingMeasurementData.RangeStatus == 0)
         {
            if (pDev->LeakyFirst)
            {
               pDev->LeakyFirst = 0;
               pDev->LeakyRange = RangingMeasurementData.RangeDMaxMilliMeter;
            }
            else
            {
               pDev->LeakyRange = (pDev->LeakyRange * LeakyFactorFix8 +
                                                              (256 - LeakyFactorFix8) * RangingMeasurementData.RangeDMaxMilliMeter) >> 8;
            }
#if defined(STEMWIN)
            sprintf(measure_str, "%d mm", RangingMeasurementData.RangeMilliMeter);
            GUI__DispStringInRect(measure_str, &pGUI->text_rect, GUI_TA_CENTER | GUI_TA_VCENTER, strlen(measure_str));
#endif
         }
         else
         {
#if defined(STEMWIN)
            sprintf(measure_str, "~~~ mm");
            GUI__DispStringInRect(measure_str, &pGUI->text_rect, GUI_TA_CENTER | GUI_TA_VCENTER, strlen(measure_str));
#endif
            pDev->LeakyFirst = 1;
         }
         osDelay(V530L0X_SINGLE_DELAY);
      }
      else if (pDev->running_mode == VL53L0X_RUNNING_CONTINUOUS_MODE)
      {
         if (xSemaphoreTake(g_53l0a1_right_event_Semaphore, 1000) == pdPASS)
         {
         }
         else
         {
            VL53L0X_StopMeasurement(pDev);                  // it is safer to do this while sensor is stopped
            VL53L0X_ClearInterruptMask(pDev, -1);

            // Start continuous ranging
            VL53L0X_StartMeasurement(pDev);
            debug_output_info("... \r\n");
         }
      }
      else
      {
         osDelay(1000);
      }
   }
}

#endif      // RTOS_FREERTOS


/* --------------------------------------------------------------------------
 * Name : vl53l0x_init()
 *
 *
 * -------------------------------------------------------------------------- */
int vl53l0x_init(VL53L0X_Dev_t *pDev)
{
   int status                                            = 0;
   uint8_t VhvSettings;
   uint8_t PhaseCal;
   uint32_t refSpadCount;
   uint8_t isApertureSpads;

   UNUSED(status);

   // Initialize the device in continuous ranging mode
   VL53L0X_StaticInit(pDev);
   VL53L0X_PerformRefCalibration(pDev, &VhvSettings, &PhaseCal);
   VL53L0X_PerformRefSpadManagement(pDev, &refSpadCount, &isApertureSpads);
   VL53L0X_SetInterMeasurementPeriodMilliSeconds(pDev, 100);
   VL53L0X_SetDeviceMode(pDev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);

   debug_output_info("V53L0X-A0[%d] cal data %d, %d \r\n", pDev->Id, VhvSettings, PhaseCal);

   // et sensor interrupt mode
   VL53L0X_StopMeasurement(pDev);           // it is safer to do this while sensor is stopped
   VL53L0X_SetInterruptThresholds(pDev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING, 0 << 16 , 300 << 16);

   status                                                = VL53L0X_SetGpioConfig(pDev, 0, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING, VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_HIGH, VL53L0X_INTERRUPTPOLARITY_HIGH);
   status                                                = VL53L0X_ClearInterruptMask(pDev, -1);         // clear interrupt pending if any

   // Start continuous ranging
   VL53L0X_StartMeasurement(pDev);


   return 0;
}




#if 0
VL53L0X_RangingMeasurementData_t RangingMeasurementData;
int vl53l0x_init(VL53L0X_Dev_t *pDev)
{
   int status                                            = 0;
   uint8_t VhvSettings;
   uint8_t PhaseCal;
   uint32_t refSpadCount;
   uint8_t isApertureSpads;

   FixPoint1616_t signalLimit                            = (FixPoint1616_t) (0.25 * 65536);
   FixPoint1616_t sigmaLimit                             = (FixPoint1616_t) (18 * 65536);
   uint32_t timingBudget                                 = 33000;
   uint8_t preRangeVcselPeriod                           = 14;
   uint8_t finalRangeVcselPeriod                         = 10;

   status                                                = VL53L0X_StaticInit(pDev);
   if (status)
   {
      debug_output_error("VL53L0X_StaticInit %d failed \r\n", pDev->Id);
   }

   status                                                = VL53L0X_PerformRefCalibration(pDev, &VhvSettings, &PhaseCal);
   if (status)
   {
      debug_output_error("VL53L0X_PerformRefCalibration failed \r\n");
   }
   debug_output_info("V53L0X-A0[%d] cal data %d, %d \r\n", pDev->Id, VhvSettings, PhaseCal);

   status                                                = VL53L0X_PerformRefSpadManagement(pDev, &refSpadCount, &isApertureSpads);
   if (status)
   {
      debug_output_error("VL53L0X_PerformRefSpadManagement failed \r\n");
   }
   debug_output_info("V53L0X-A0[%d] VL53L0X_PerformRefSpadManagement %d, %d \r\n", pDev->Id, (int) refSpadCount, (int) isApertureSpads);

   // Setup in single ranging mode
   status                                                = VL53L0X_SetDeviceMode(pDev, VL53L0X_DEVICEMODE_SINGLE_RANGING);
   if (status)
   {
      debug_output_error("VL53L0X_SetDeviceMode failed \r\n");
   }

   // Enable Sigma limit
   status                                                = VL53L0X_SetLimitCheckEnable(pDev, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
   if (status)
   {
      debug_output_error("VL53L0X_SetLimitCheckEnable failed \r\n");
   }

   // Enable Signa limit
   status                                                = VL53L0X_SetLimitCheckEnable(pDev, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
   if (status)
   {
      debug_output_error("VL53L0X_SetLimitCheckEnable failed \r\n");
   }

   // Ranging configuration */
#if 1
   // LONG_RANGE:
   signalLimit                                           = (FixPoint1616_t) (0.1 * 65536);
   sigmaLimit                                            = (FixPoint1616_t) (60 * 65536);
   timingBudget                                          = 33000;
   preRangeVcselPeriod                                   = 18;
   finalRangeVcselPeriod                                 = 14;
#endif

#if 1
   // HIGH_ACCURACY:
   signalLimit                                           = (FixPoint1616_t) (0.25 * 65536);
   sigmaLimit                                            = (FixPoint1616_t) (18 * 65536);
   timingBudget                                          = 200000;
   preRangeVcselPeriod                                   = 14;
   finalRangeVcselPeriod                                 = 10;
#endif

#if 1
   // HIGH_SPEED:
   signalLimit                                           = (FixPoint1616_t) (0.25 * 65536);
   sigmaLimit                                            = (FixPoint1616_t) (32 * 65536);
   timingBudget                                          = 20000;
   preRangeVcselPeriod                                   = 14;
   finalRangeVcselPeriod                                 = 10;
#endif

   status                                                = VL53L0X_SetLimitCheckValue(pDev, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, signalLimit);
   if (status)
   {
      debug_output_error("VL53L0X_SetLimitCheckValue failed \r\n");
   }

   status                                                = VL53L0X_SetLimitCheckValue(pDev, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, sigmaLimit);
   if (status)
   {
      debug_output_error("VL53L0X_SetLimitCheckValue failed \r\n");
   }

   status                                                = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(pDev, timingBudget);
   if (status)
   {
      debug_output_error("VL53L0X_SetMeasurementTimingBudgetMicroSeconds failed \r\n");
   }

   status                                                = VL53L0X_SetVcselPulsePeriod(pDev, VL53L0X_VCSEL_PERIOD_PRE_RANGE, preRangeVcselPeriod);
   if (status)
   {
      debug_output_error("VL53L0X_SetVcselPulsePeriod failed \r\n");
   }

   status                                                = VL53L0X_SetVcselPulsePeriod(pDev, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, finalRangeVcselPeriod);
   if (status)
   {
      debug_output_error("VL53L0X_SetVcselPulsePeriod failed \r\n");
   }

   status                                                = VL53L0X_PerformRefCalibration(pDev, &VhvSettings, &PhaseCal);
   if (status)
   {
      debug_output_error("VL53L0X_PerformRefCalibration failed \r\n");
   }


   // Call All-In-One blocking API function */
   status                                                = VL53L0X_PerformSingleRangingMeasurement(pDev, &RangingMeasurementData);
   if (status == 0)
   {
      // Push data logging to UART
      // trace_printf("%d,%u,%d,%d,%d\n", VL53L0XDevs[SingleSensorNo].Id, TimeStamp_Get(), RangingMeasurementData.RangeStatus, RangingMeasurementData.RangeMilliMeter, RangingMeasurementData.SignalRateRtnMegaCps);
      // Sensor_SetNewRange(&VL53L0XDevs[SingleSensorNo],&RangingMeasurementData);
      // Display distance in cm
      if (RangingMeasurementData.RangeStatus == 0)
      {
         // sprintf(StrDisplay, "%3dc",(int)VL53L0XDevs[SingleSensorNo].LeakyRange/10);
      }
      else
      {
//         sprintf(StrDisplay, "----");
//         StrDisplay[0]=VL53L0XDevs[SingleSensorNo].DevLetter;
      }
   }


#endif



#if 0
/* --------------------------------------------------------------------------
 * Name : vl53l0x_init()
 *
 *
 * -------------------------------------------------------------------------- */
#if 1
VL53L0X_RangingMeasurementData_t RangingMeasurementData;
int vl53l0x_init(VL53L0X_Dev_t *pDev)
{
   int status                                            = 0;
   uint8_t VhvSettings;
   uint8_t PhaseCal;
   uint32_t refSpadCount;
   uint8_t isApertureSpads;

   FixPoint1616_t signalLimit                            = (FixPoint1616_t) (0.25 * 65536);
   FixPoint1616_t sigmaLimit                             = (FixPoint1616_t) (18 * 65536);
   uint32_t timingBudget                                 = 33000;
   uint8_t preRangeVcselPeriod                           = 14;
   uint8_t finalRangeVcselPeriod                         = 10;

   status                                                = VL53L0X_StaticInit(pDev);
   if (status)
   {
      debug_output_error("VL53L0X_StaticInit %d failed \r\n", pDev->Id);
   }

   status                                                = VL53L0X_PerformRefCalibration(pDev, &VhvSettings, &PhaseCal);
   if (status)
   {
      debug_output_error("VL53L0X_PerformRefCalibration failed \r\n");
   }
   debug_output_info("V53L0X-A0[%d] cal data %d, %d \r\n", pDev->Id, VhvSettings, PhaseCal);

   status                                                = VL53L0X_PerformRefSpadManagement(pDev, &refSpadCount, &isApertureSpads);
   if (status)
   {
      debug_output_error("VL53L0X_PerformRefSpadManagement failed \r\n");
   }
   debug_output_info("V53L0X-A0[%d] VL53L0X_PerformRefSpadManagement %d, %d \r\n", pDev->Id, (int) refSpadCount, (int) isApertureSpads);

   // Setup in single ranging mode
   status                                                = VL53L0X_SetDeviceMode(pDev, VL53L0X_DEVICEMODE_SINGLE_RANGING);
   if (status)
   {
      debug_output_error("VL53L0X_SetDeviceMode failed \r\n");
   }

   // Enable Sigma limit
   status                                                = VL53L0X_SetLimitCheckEnable(pDev, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
   if (status)
   {
      debug_output_error("VL53L0X_SetLimitCheckEnable failed \r\n");
   }

   // Enable Signa limit
   status                                                = VL53L0X_SetLimitCheckEnable(pDev, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
   if (status)
   {
      debug_output_error("VL53L0X_SetLimitCheckEnable failed \r\n");
   }

   // Ranging configuration */
#if 1
   // LONG_RANGE:
   signalLimit                                           = (FixPoint1616_t) (0.1 * 65536);
   sigmaLimit                                            = (FixPoint1616_t) (60 * 65536);
   timingBudget                                          = 33000;
   preRangeVcselPeriod                                   = 18;
   finalRangeVcselPeriod                                 = 14;
#endif

#if 1
   // HIGH_ACCURACY:
   signalLimit                                           = (FixPoint1616_t) (0.25 * 65536);
   sigmaLimit                                            = (FixPoint1616_t) (18 * 65536);
   timingBudget                                          = 200000;
   preRangeVcselPeriod                                   = 14;
   finalRangeVcselPeriod                                 = 10;
#endif

#if 1
   // HIGH_SPEED:
   signalLimit                                           = (FixPoint1616_t) (0.25 * 65536);
   sigmaLimit                                            = (FixPoint1616_t) (32 * 65536);
   timingBudget                                          = 20000;
   preRangeVcselPeriod                                   = 14;
   finalRangeVcselPeriod                                 = 10;
#endif

   status                                                = VL53L0X_SetLimitCheckValue(pDev, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, signalLimit);
   if (status)
   {
      debug_output_error("VL53L0X_SetLimitCheckValue failed \r\n");
   }

   status                                                = VL53L0X_SetLimitCheckValue(pDev, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, sigmaLimit);
   if (status)
   {
      debug_output_error("VL53L0X_SetLimitCheckValue failed \r\n");
   }

   status                                                = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(pDev, timingBudget);
   if (status)
   {
      debug_output_error("VL53L0X_SetMeasurementTimingBudgetMicroSeconds failed \r\n");
   }

   status                                                = VL53L0X_SetVcselPulsePeriod(pDev, VL53L0X_VCSEL_PERIOD_PRE_RANGE, preRangeVcselPeriod);
   if (status)
   {
      debug_output_error("VL53L0X_SetVcselPulsePeriod failed \r\n");
   }

   status                                                = VL53L0X_SetVcselPulsePeriod(pDev, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, finalRangeVcselPeriod);
   if (status)
   {
      debug_output_error("VL53L0X_SetVcselPulsePeriod failed \r\n");
   }

   status                                                = VL53L0X_PerformRefCalibration(pDev, &VhvSettings, &PhaseCal);
   if (status)
   {
      debug_output_error("VL53L0X_PerformRefCalibration failed \r\n");
   }


   // Call All-In-One blocking API function */
   status                                                = VL53L0X_PerformSingleRangingMeasurement(pDev, &RangingMeasurementData);
   if (status == 0)
   {
      // Push data logging to UART
      // trace_printf("%d,%u,%d,%d,%d\n", VL53L0XDevs[SingleSensorNo].Id, TimeStamp_Get(), RangingMeasurementData.RangeStatus, RangingMeasurementData.RangeMilliMeter, RangingMeasurementData.SignalRateRtnMegaCps);
      // Sensor_SetNewRange(&VL53L0XDevs[SingleSensorNo],&RangingMeasurementData);
      // Display distance in cm
      if (RangingMeasurementData.RangeStatus == 0)
      {
         // sprintf(StrDisplay, "%3dc",(int)VL53L0XDevs[SingleSensorNo].LeakyRange/10);
      }
      else
      {
//         sprintf(StrDisplay, "----");
//         StrDisplay[0]=VL53L0XDevs[SingleSensorNo].DevLetter;
      }
   }


// 531
   return 0;
}

#else
struct AlrmMode_t
{
   const int VL53L0X_Mode;
   const char *Name;
   uint32_t ThreshLow;
   uint32_t ThreshHigh;
};

struct AlrmMode_t AlarmModes[]                           =
{
   { .VL53L0X_Mode = VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_LOW , .Name="Lo" , .ThreshLow=300<<16 ,  .ThreshHigh=0<<16  },
   { .VL53L0X_Mode = VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_HIGH, .Name= "hi", .ThreshLow=0<<16   ,  .ThreshHigh=300<<16},
   { .VL53L0X_Mode = VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_OUT , .Name="out", .ThreshLow=300<<16 ,  .ThreshHigh=400<<16},
};
int vl53l0x_init(VL53L0X_Dev_t *pDev)
{
   uint8_t VhvSettings;
   uint8_t PhaseCal;
   uint32_t refSpadCount;
   uint8_t isApertureSpads;
   VL53L0X_RangingMeasurementData_t RangingMeasurementData;
   int status;
   int Over                                              = 0;
   int Mode                                              = 0;

   // Initialize the device in continuous ranging mode
   VL53L0X_StaticInit(pDev);
   VL53L0X_PerformRefCalibration(pDev, &VhvSettings, &PhaseCal);
   VL53L0X_PerformRefSpadManagement(pDev, &refSpadCount, &isApertureSpads);
   VL53L0X_SetInterMeasurementPeriodMilliSeconds(pDev, 250);
   VL53L0X_SetDeviceMode(pDev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);

   // et sensor interrupt mode
   VL53L0X_StopMeasurement(pDev);           // it is safer to do this while sensor is stopped
   VL53L0X_SetInterruptThresholds(pDev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING , AlarmModes[Mode].ThreshLow , AlarmModes[Mode].ThreshHigh);

   status                                                = VL53L0X_SetGpioConfig(pDev, 0, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING, AlarmModes[Mode].VL53L0X_Mode, VL53L0X_INTERRUPTPOLARITY_HIGH);
   status                                                = VL53L0X_ClearInterruptMask(pDev, -1);         // clear interrupt pending if any

   // Start continuous ranging
   VL53L0X_StartMeasurement(pDev);

   __WFI();
   // Interrupt received
//   if (IntrCounts[1] != 0)
//   {
      // Reset interrupt counter
//      IntrCounts[1] = 0;
      // Get ranging data and display distance
      VL53L0X_GetRangingMeasurementData(pDev, &RangingMeasurementData);
//      sprintf(StrDisplay, "%3dc",(int)RangingMeasurementData.RangeMilliMeter/10);
      // Clear interrupt
      status                                             = VL53L0X_ClearInterruptMask(pDev, -1);
//   }

   // Stop continuous ranging
   VL53L0X_StopMeasurement(pDev);
   // Stop continuous ranging
   VL53L0X_StopMeasurement(pDev);

   return 0;
}
#endif
#endif










#endif   // P_NUCLEO_53L0A1

#if defined(RTOS_FREERTOS)
/* --------------------------------------------------------------------------
 * Name : touch_event_task()
 *
 *
 * -------------------------------------------------------------------------- */
void stmpe1600_test_task(void const* argument)
{
static int test_count                                    = 0;
static char szString[6];
   while (1)
   {
      osDelay(1000);
      xSemaphoreTake(g_53l0a1_semaphore, portMAX_DELAY);
      sprintf(szString, "%04d", test_count++);
#if defined(GPIO_STMPE1600)
      XNUCLEO53L1A1_SetDisplayString(szString);
#endif
      xSemaphoreGive(g_53l0a1_semaphore);
   }
}
#endif

/* --------------------------------------------------------------------------
 * Name : BSP_I2C_BUS3_Read()
 *
 *
 * -------------------------------------------------------------------------- */
int BSP_I2C_BUS3_Read(uint16_t device_addr, uint16_t reg_addr, uint16_t reg_addr_size, uint8_t* pData, uint16_t DataSize)
{
   HAL_StatusTypeDef status                              = HAL_OK;
   status                                                = HAL_I2C_Mem_Read(&g_I2C_Bus3_handle, device_addr, (uint16_t) reg_addr, reg_addr_size, pData, DataSize, 1000);

   // Check the communication status
   if (status != HAL_OK)
   {
      debug_output_error("HAL_I2C_Mem_Read Failed() : %d \r\n", status);
      return -1;
   }
   return 0;
}

/* --------------------------------------------------------------------------
 * Name : BSP_I2C_BUS3_Write()
 *
 *
 * -------------------------------------------------------------------------- */
int BSP_I2C_BUS3_Write(uint16_t device_addr, uint16_t reg_addr, uint16_t reg_addr_size, uint8_t* pData, uint16_t DataSize)
{
   HAL_StatusTypeDef status                              = HAL_OK;
   status                                                = HAL_I2C_Mem_Write(&g_I2C_Bus3_handle, device_addr, (uint16_t) reg_addr, reg_addr_size, pData, DataSize, 1000);

   // Check the communication status
   if (status != HAL_OK)
   {
      debug_output_error("HAL_I2C_Mem_Read Failed() : %d \r\n", status);
      return -1;
   }
   return 0;
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

/* --------------------------------------------------------------------------
 * Name : Display_Clear()
 *
 *
 * -------------------------------------------------------------------------- */
void Display_Clear(void)
{
   if (HAL_DMA2D_Start(&g_DMA2D_handle, 0x00000000, (uint32_t) LCD_FRAMEBUFFER_LAYER_0, LCD_WIDTH, LCD_HEIGHT) == HAL_OK)
   {
      /* Polling For DMA transfer */  
      HAL_DMA2D_PollForTransfer(&g_DMA2D_handle, 10);
   }
   else
   {
      debug_output_info("MX_DMA2D_LCD_Clear() failed !!! \r\n");
   }
}

/* --------------------------------------------------------------------------
 * Name : LCD_Clear()
 *
 *
 * -------------------------------------------------------------------------- */
volatile uint32_t* getFrameBuffer(void)
{
   return ((volatile uint32_t*) LCD_FRAMEBUFFER_LAYER_0);
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
   // initialize LCD
   BSP_LTDC_Init();
   // Initialize DMA 2D for LCD
   BSP_DMA2D_Init();

   // clear
   Display_Clear();

#if defined(STEMWIN)
   __HAL_RCC_CRC_CLK_ENABLE();                                                         // Enable the CRC Module
   lcd_init_for_stemwin((void *) &g_LTDC_handle, (void *) &g_DMA2D_handle);
   GUI_Init();

   Display_On(1);

   GUI_DispStringAt("Starting...", 0, 0);

   // ctivate the use of memory device feature
   WM_SetCreateFlags(WM_CF_MEMDEV);
#endif

#if defined(TIMER_TEST)
   test_timer1();
   test_timer2();
   test_timer12();
#endif

   // I2C BUS 3
   BSP_I2C_BUS3_Init();

#if defined(FT5536)
   touch_ft5536_init(BSP_I2C_BUS3_Read, BSP_I2C_BUS3_Write);

    // -------------------------------------------------------------------------
    // touch interrupt
    GPIO_InitStruct.Pin                                   = TOUCH_FT5536_INT_PIN;
    GPIO_InitStruct.Mode                                  = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull                                  = GPIO_NOPULL;
    GPIO_InitStruct.Speed                                 = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(TOUCH_FT5536_INT_PORT, &GPIO_InitStruct);

#if defined(RTOS_FREERTOS)
   // create a binary semaphore used for informing ethernetif of frame reception
   g_touch_event_Semaphore                               = xSemaphoreCreateBinary();

   // --------------------------------------------------------------------------
   // Thread definition for touch
   osThreadDef(touch_event_task, touch_event_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 3);
   if (osThreadCreate(osThread(touch_event_task), (void *) NULL) == NULL)
   {
      debug_output_error("Can't create thread : touch_event_task !!! \r\n");
   }
#endif      // RTOS_FREERTOS

    // EXTI interrupt init for PJ4 (EXTI15_10_IRQn)
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

   touch_start(TOUCH_FT5536_TRIGGER_MODE_INTERRUPT);
#endif      // FT5536

   // initialize I2C1
   BSP_I2C_BUS1_Init();
#if defined(RTOS_FREERTOS)
   // create a binary semaphore used for informing ethernetif of frame reception
//   g_i2c_bus_1_semaphore                                 = xSemaphoreCreateBinary();

   g_i2c_bus_1_semaphore                                 = xSemaphoreCreateMutex();

   if (g_i2c_bus_1_semaphore == NULL)
   {
      debug_output_error("Can't create semaphore !!! \r\n");
      Error_Handler();
   }

   g_53l0a1_semaphore                                    = xSemaphoreCreateMutex();

#endif   // RTOS_FREERTOS

#if defined(GPIO_STMPE1600)
   // initialize stmpe1600
#if defined(RTOS_FREERTOS)
   if (gpio_stmpe1600_init((void *) &g_I2C_Bus1_handle, (void *) &g_i2c_bus_1_semaphore) < 0)
   {
      return;
   }
#else
   if (gpio_stmpe1600_init((void *) &g_I2C_Bus1_handle, NULL) < 0)
   {
      return;
   }
#endif

#if defined(RTOS_FREERTOS)
   // --------------------------------------------------------------------------
   // Thread definition for stmpe1600
   osThreadDef(stmpe1600_test_task, stmpe1600_test_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
   if (osThreadCreate(osThread(stmpe1600_test_task), (void *) NULL) == NULL)
   {
      debug_output_error("Can't create thread : stmpe1600_test_task !!! \r\n");
   }
#endif   // RTOS_FREERTOS

#if defined(RTOS_FREERTOS)
   VL53L0XDevs[VL53L0_A1_CENTER_PORT].i2c_semaphore      = (void *) &g_i2c_bus_1_semaphore;
   VL53L0XDevs[VL53L0_A1_LEFT_PORT].i2c_semaphore        = (void *) &g_i2c_bus_1_semaphore;
   VL53L0XDevs[VL53L0_A1_RIGHT_PORT].i2c_semaphore       = (void *) &g_i2c_bus_1_semaphore;
#endif

#if defined(P_NUCLEO_53L0A1)
   if (Nucleo_53l0a1_init() < 0)
   {
      debug_output_error("Nucleo_53l0a1_init() failed \r\n");
   }

#if 1
   VL53L0XDevs[VL53L0_A1_CENTER_PORT].running_mode       = VL53L0X_RUNNING_SINGLE_SHOT_MODE;
   VL53L0XDevs[VL53L0_A1_LEFT_PORT].running_mode         = VL53L0X_RUNNING_SINGLE_SHOT_MODE;
   VL53L0XDevs[VL53L0_A1_RIGHT_PORT].running_mode        = VL53L0X_RUNNING_SINGLE_SHOT_MODE;


   VL53L0XDevs[VL53L0_A1_CENTER_PORT].single_shot_option = VL53L0X_RUNNING_SINGLE_SHOT_HIGH_ACCURACY;
   VL53L0XDevs[VL53L0_A1_LEFT_PORT].single_shot_option   = VL53L0X_RUNNING_SINGLE_SHOT_HIGH_ACCURACY;
   VL53L0XDevs[VL53L0_A1_RIGHT_PORT].single_shot_option  = VL53L0X_RUNNING_SINGLE_SHOT_LONG_RANGE;

//   VL53L0XDevs[VL53L0_A1_CENTER_PORT].single_shot_option = VL53L0X_RUNNING_SINGLE_SHOT_LONG_RANGE;
//   VL53L0XDevs[VL53L0_A1_LEFT_PORT].single_shot_option   = VL53L0X_RUNNING_SINGLE_SHOT_HIGH_ACCURACY;
//   VL53L0XDevs[VL53L0_A1_RIGHT_PORT].single_shot_option  = VL53L0X_RUNNING_SINGLE_SHOT_HIGH_SPEED;
#else
   VL53L0XDevs[VL53L0_A1_CENTER_PORT].running_mode       = VL53L0X_RUNNING_CONTINUOUS_MODE;
   VL53L0XDevs[VL53L0_A1_LEFT_PORT].running_mode         = VL53L0X_RUNNING_CONTINUOUS_MODE;
   VL53L0XDevs[VL53L0_A1_RIGHT_PORT].running_mode        = VL53L0X_RUNNING_CONTINUOUS_MODE;

   VL53L0XDevs[VL53L0_A1_CENTER_PORT].single_shot_option = 0;
   VL53L0XDevs[VL53L0_A1_LEFT_PORT].single_shot_option   = 0;
   VL53L0XDevs[VL53L0_A1_RIGHT_PORT].single_shot_option  = 0;
#endif


   // -------------------------------------------------------------------------------------------------------
   // Initialize
   if (VL53L0XDevs[VL53L0_A1_CENTER_PORT].running_mode == VL53L0X_RUNNING_SINGLE_SHOT_MODE)
   {
      vl53l0x_Single_Mode_init(&VL53L0XDevs[VL53L0_A1_CENTER_PORT]);
   }
   else
   {
      vl53l0x_Continuous_Mode_init(&VL53L0XDevs[VL53L0_A1_CENTER_PORT]);
   }

   if (VL53L0XDevs[VL53L0_A1_LEFT_PORT].running_mode == VL53L0X_RUNNING_SINGLE_SHOT_MODE)
   {
      vl53l0x_Single_Mode_init(&VL53L0XDevs[VL53L0_A1_LEFT_PORT]);
   }
   else
   {
      vl53l0x_Continuous_Mode_init(&VL53L0XDevs[VL53L0_A1_LEFT_PORT]);
   }

   if (VL53L0XDevs[VL53L0_A1_RIGHT_PORT].running_mode == VL53L0X_RUNNING_SINGLE_SHOT_MODE)
   {
      vl53l0x_Single_Mode_init(&VL53L0XDevs[VL53L0_A1_RIGHT_PORT]);
   }
   else
   {
      vl53l0x_Continuous_Mode_init(&VL53L0XDevs[VL53L0_A1_RIGHT_PORT]);
   }

#if defined(RTOS_FREERTOS)
    // -------------------------------------------------------------------------
    // vl53l0 interrupt
    GPIO_InitStruct.Pin                                   = P_NUCLEO_53L0A1_LEFT_INT_PIN;
    GPIO_InitStruct.Mode                                  = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull                                  = GPIO_NOPULL;
    GPIO_InitStruct.Speed                                 = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(P_NUCLEO_53L0A1_LEFT_INT_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin                                   = P_NUCLEO_53L0A1_RIGHT_INT_PIN;
    GPIO_InitStruct.Mode                                  = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull                                  = GPIO_NOPULL;
    GPIO_InitStruct.Speed                                 = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(P_NUCLEO_53L0A1_RIGHT_INT_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin                                   = P_NUCLEO_53L0A1_CENTER_INT_PIN;
    GPIO_InitStruct.Mode                                  = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull                                  = GPIO_NOPULL;
    GPIO_InitStruct.Speed                                 = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(P_NUCLEO_53L0A1_CENTER_INT_PORT, &GPIO_InitStruct);

    // --------------------------------------------------------------------------
    // semaphore
#if 1
    g_53l0a1_left_event_Semaphore                        = xSemaphoreCreateMutex();;
    g_53l0a1_center_event_Semaphore                      = xSemaphoreCreateMutex();;
    g_53l0a1_right_event_Semaphore                       = xSemaphoreCreateMutex();;
#else
    g_53l0a1_left_event_Semaphore                        = xSemaphoreCreateBinary();;
    g_53l0a1_center_event_Semaphore                      = xSemaphoreCreateBinary();;
    g_53l0a1_right_event_Semaphore                       = xSemaphoreCreateBinary();;
#endif

    // --------------------------------------------------------------------------
    // Thread definition for v53l0a1
    osThreadDef(v53l0a1_center_event_task, v53l0a1_center_event_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 5);
    if (osThreadCreate(osThread(v53l0a1_center_event_task), (void *) NULL) == NULL)
    {
       debug_output_error("Can't create thread : touch_event_task !!!");
    }

    // --------------------------------------------------------------------------
    // Thread definition for v53l0a1
    osThreadDef(v53l0a1_left_event_task, v53l0a1_left_event_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 5);
    if (osThreadCreate(osThread(v53l0a1_left_event_task), (void *) NULL) == NULL)
    {
       debug_output_error("Can't create thread : touch_event_task !!!");
    }

    // --------------------------------------------------------------------------
    // Thread definition for v53l0a1
    osThreadDef(v53l0a1_right_event_task, v53l0a1_right_event_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 5);
    if (osThreadCreate(osThread(v53l0a1_right_event_task), (void *) NULL) == NULL)
    {
       debug_output_error("Can't create thread : touch_event_task !!!");
    }

    // EXTI interrupt init for ...
    HAL_NVIC_SetPriority(EXTI2_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);

    // EXTI interrupt init for ...
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
#endif   // RTOS_FREERTOS
#endif   // P_NUCLEO_53L0A1
#endif   // GPIO_STMPE1600

#if defined(NET_LWIP)
   // Initialize Network
   BSP_LWIP_Init();
#endif

}



