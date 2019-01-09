/*
 * File: main.cpp
 *
 * Written by duvallee.lee in 2018
 *
 */
#include "main.h"

#if defined(STEVAL_FCU001V1)
#include "steval_fcu001v1_driver.h"
#endif

#if defined(DISCOVERY_STM32F7)
#include "discovery_stm32f7_driver.h"
#endif

#if defined(DISCOVERY_STM32F7_BOOTLOADER)
#include "discovery_stm32f7_driver.h"
#endif

#if defined(NUCLEO_H743ZI)
#include "nucleo_h743zi_driver.h"
#endif

#if (defined(USE_USB_CDC_DEVICE) || defined(USE_USB_BULK_DEVICE))
#include "usb_device.h"
#endif

#if defined(CLEANFLIGHT_CLI)
#include "cleanflight_cli.h"
#endif

#if defined(QSPI_FLASH_USE)
#include "qspi_flash.h"
#endif

#if (defined(NUCLEO_H743ZI) || defined(DISCOVERY_STM32F7) || defined(DISCOVERY_STM32F7_BOOTLOADER))
/* --------------------------------------------------------------------------
 * Name : CPU_CACHE_Enable()
 *
 *  @ brief  CPU L1-Cache enable.
 *  @ param  None
 *  @ retval None
 *
 * -------------------------------------------------------------------------- */
static void CPU_CACHE_Enable(void)
{
   // Invalidate I-Cache : ICIALLU register
   SCB_InvalidateICache();

   // Enable branch prediction
   SCB->CCR                                              |= (1 << 18);
   __DSB();

   // Invalidate I-Cache : ICIALLU register
   SCB_InvalidateICache();

   // Enable I-Cache
   SCB_EnableICache();

   // Invalidate D-Cache
   SCB_InvalidateDCache();
   // Enable D-Cache
   SCB_EnableDCache();
}
#endif

#if (defined(DISCOVERY_STM32F7) || defined(DISCOVERY_STM32F7_BOOTLOADER))
/* --------------------------------------------------------------------------
 * Name : MPU_Config()
 *
 *
 * -------------------------------------------------------------------------- */
static void MPU_Config(void)
{
   MPU_Region_InitTypeDef MPU_InitStruct;

   // Disable the MPU
   HAL_MPU_Disable();

   // Configure the MPU attributes as Device for Ethernet Descriptors in the SRAM 
   MPU_InitStruct.Enable                                 = MPU_REGION_ENABLE;
   MPU_InitStruct.BaseAddress                            = ETHERNET_SRAM_BASE_ADDRESS;
   MPU_InitStruct.Size                                   = MPU_REGION_SIZE_256KB;
   MPU_InitStruct.AccessPermission                       = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.IsBufferable                           = MPU_ACCESS_BUFFERABLE;
   MPU_InitStruct.IsCacheable                            = MPU_ACCESS_NOT_CACHEABLE;
   MPU_InitStruct.IsShareable                            = MPU_ACCESS_SHAREABLE;
   MPU_InitStruct.Number                                 = MPU_REGION_NUMBER0;
   MPU_InitStruct.TypeExtField                           = MPU_TEX_LEVEL0;
   MPU_InitStruct.SubRegionDisable                       = 0x00;
   MPU_InitStruct.DisableExec                            = MPU_INSTRUCTION_ACCESS_ENABLE;

   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   // Enable D-cache on SDRAM (Write-through)
   MPU_InitStruct.Enable                                 = MPU_REGION_ENABLE;
   MPU_InitStruct.BaseAddress                            = 0xC0000000;
   MPU_InitStruct.Size                                   = MPU_REGION_SIZE_8MB;
   MPU_InitStruct.AccessPermission                       = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.IsBufferable                           = MPU_ACCESS_NOT_BUFFERABLE;
   MPU_InitStruct.IsCacheable                            = MPU_ACCESS_CACHEABLE;
   MPU_InitStruct.IsShareable                            = MPU_ACCESS_SHAREABLE;
   MPU_InitStruct.Number                                 = MPU_REGION_NUMBER0;
   MPU_InitStruct.TypeExtField                           = MPU_TEX_LEVEL0;
   MPU_InitStruct.SubRegionDisable                       = 0x00;
   MPU_InitStruct.DisableExec                            = MPU_INSTRUCTION_ACCESS_DISABLE;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   // Enable the MPU
   HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
#endif

/* --------------------------------------------------------------------------
 * Name : debug_break()
 *
 *
 * -------------------------------------------------------------------------- */
int __attribute__ ((noinline)) debug_break(int data)
{
static volatile int debug_break_count                    = 0;
   UNUSED((void) debug_break_count);

   debug_break_count                                     = data;
   return 0;
}

#if defined(STM32H743xx)
/* --------------------------------------------------------------------------
 * Name : power_on()
 *
 *
 * -------------------------------------------------------------------------- */
void power_on(void)
{
}
#endif

#if defined(RTOS_FREERTOS)
#if (configAPPLICATION_ALLOCATED_HEAP == 1)
#if 0
uint8_t* ucHeap                                          = (uint8_t *) 0xC0000000;
#else
uint8_t ucHeap[configTOTAL_HEAP_SIZE]                    = {0, };
#endif
#endif

void freertos_idle_task(void const* argument)
{
#if defined(STEMWIN)
   GUI_Clear();
   GUI_SetBkColor(GUI_DARKGRAY);
#endif
   while (1)
   {
      osDelay(10000);
      debug_output_info("================== IDLE TASK ================== \r\n");
#if 0
      GUI_SetPenSize(3);
      GUI_SetColor(GUI_GREEN);
      
      GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
      
      GUI_DrawRoundedRect(  0, 200, 159, 269, 10);
      GUI_DrawRoundedRect(160, 200, 319, 269, 10);
      GUI_DrawRoundedRect(320, 200, 479, 269, 10);

      GUI_SetFont(&GUI_Font10_1);
      GUI_SetColor(GUI_CYAN);
      GUI_DispStringAt("Left : ", 2, 235);
      GUI_DispStringAt("Center : ", 162, 235);
      GUI_DispStringAt("Right : ", 322, 235);
#endif
   }
}
#endif

/* --------------------------------------------------------------------------
 * Name : main()
 *
 *
 * -------------------------------------------------------------------------- */
int main(void)
{
   // Configure the MPU attributes as Write Through
#if (defined(DISCOVERY_STM32F7) || defined(DISCOVERY_STM32F7_BOOTLOADER))
   MPU_Config();
#endif

#if (defined(NUCLEO_H743ZI) || defined(DISCOVERY_STM32F7) || defined(DISCOVERY_STM32F7_BOOTLOADER))
   CPU_CACHE_Enable();
#endif

   // Reset of all peripherals, Initializes the Flash interface and the Systick.
   HAL_Init();

   // Configure the system clock
   SystemClock_Config();

#if defined(STM32H743xx)
   power_on();
#endif

   debug_break(100);

#if defined(UART_DEBUG_OUTPUT)
   uart_debug_init();
#endif

   debug_output_info("=============================================== \r\n");
   debug_output_info("%s Ver%d.%d.%d \r\n", PROGRAM_NAME, VERSION_MAIN, VERSION_MINOR, VERSION_SUB);
   debug_output_info("Build Date : %s %s (%s) \r\n", __DATE__, __TIME__, __VERSION__);
   debug_output_info("=============================================== \r\n\r\n");

#if defined(QSPI_FLASH_USE)
   // Initialize the NOR
   if (qspi_flash_init() < 0)
   {
      debug_output_error("qspi_flash_init() failed !!! \r\n");
      Error_Handler();
   }
#if defined(DISCOVERY_STM32F7)
   qspi_flash_EnableMemoryMappedMode();
#endif   // DISCOVERY_STM32F7
#endif   // QSPI_FLASH_USE

    Board_Driver_Init();

#if (defined(USE_USB_CDC_DEVICE) || defined(USE_USB_BULK_DEVICE))
   usb_device_init();
#endif

#if defined(CLEANFLIGHT_CLI)
   cleanflight_cliInit();
#endif

#if defined(RTOS_FREERTOS)
   // --------------------------------------------------------------------------
   /* Thread definition for tcp server */
   osThreadDef(idle_main_task, freertos_idle_task, osPriorityIdle, 0, configMINIMAL_STACK_SIZE);
   if (osThreadCreate(osThread(idle_main_task), (void *) NULL) == NULL)
   {
      debug_output_error("Can't create thread : idle_main_task !!! \r\n");
   }

   // --------------------------------------------------------------------------
   // Start scheduler */
   osKernelStart();
#else
   scheduler_init();
#endif

   while (1)
   {
#if !defined(RTOS_FREERTOS)
      scheduler_run();
#endif
   }
   return 0;
}


/* --------------------------------------------------------------------------
 * Name : _Error_Handler()
 *
 *
 * -------------------------------------------------------------------------- */
void _Error_Handler(char *file, int line)
{
   // User can add his own implementation to report the HAL error return state
   debug_output_error("Error_Handler() Lock : %s-%d \r\n", file, line);

   while(1)
   {
   }
}

