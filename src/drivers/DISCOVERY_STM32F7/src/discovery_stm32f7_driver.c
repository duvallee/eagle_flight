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

#if defined(P_NUCLEO_53L0A1)
#include "vl53l0x_platform.h"
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

#if (defined(RTOS_FREERTOS) && defined(FT5536))
SemaphoreHandle_t g_touch_event_Semaphore                = NULL;
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
      xSemaphoreGiveFromISR(g_touch_event_Semaphore, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
   }
#endif

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
   TOUCH_EVENT_STRUCT touch_event;
   while (1)
   {
      if (xSemaphoreTake(g_touch_event_Semaphore, portMAX_DELAY) == pdPASS)
      {
         taskENTER_CRITICAL();
         if (get_ft5536_event(&touch_event) > 0)
         {
            int i;
            debug_output_dump("pt=%d, ", touch_event.touch_point_num);
            GUI_CURSOR_SetPosition(touch_event.touch_point[0].x_pos, touch_event.touch_point[0].y_pos);
            GUI_CURSOR_Show();

            for (i = 0; i < touch_event.touch_point_num; i++)
            {
               debug_output_dump("[x=%3d, y=%3d, ", touch_event.touch_point[i].x_pos, touch_event.touch_point[i].y_pos);
               debug_output_dump("action=%d,", touch_event.touch_point[i].touch_action);
#if 0
               switch (touch_event.touch_point[i].touch_action)
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
#endif
               debug_output_dump("weight=%3d, ", touch_event.touch_point[i].touch_weight);
               debug_output_dump("misc=%d] ", touch_event.touch_point[i].touch_area);
            }
            debug_output_dump("%c\r\n", ' ');
         }
         taskEXIT_CRITICAL();
      }
   }
}
#endif      // RTOS_FREERTOS
#endif      // FT5536


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
//      debug_output_info("================== stmpe1600_test_task ================== \r\n");
      taskENTER_CRITICAL();
      sprintf(szString, "%04d", test_count++);
#if defined(GPIO_STMPE1600)
      XNUCLEO53L1A1_SetDisplayString(szString);
#endif
      taskEXIT_CRITICAL();
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
      debug_output_error("Can't create thread : touch_event_task !!!");
   }
#endif      // RTOS_FREERTOS

    // EXTI interrupt init for PJ4 (EXTI15_10_IRQn)
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

   touch_start(TOUCH_FT5536_TRIGGER_MODE_INTERRUPT);
#endif      // FT5536

   // initialize I2C1
   BSP_I2C_BUS1_Init();

#if defined(GPIO_STMPE1600)
   // initialize stmpe1600
   gpio_stmpe1600_init((void *) &g_I2C_Bus1_handle);

#if defined(RTOS_FREERTOS)
   // --------------------------------------------------------------------------
   // Thread definition for stmpe1600
   osThreadDef(stmpe1600_test_task, stmpe1600_test_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
   if (osThreadCreate(osThread(stmpe1600_test_task), (void *) NULL) == NULL)
   {
      debug_output_error("Can't create thread : stmpe1600_test_task !!!");
   }
#endif
#endif

#if defined(NET_LWIP)
   // Initialize Network
   BSP_LWIP_Init();
#endif

}



