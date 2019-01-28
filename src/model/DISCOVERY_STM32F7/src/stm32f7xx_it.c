/**
  ******************************************************************************
  * @file    stm32f7xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#include "main.h"

/* --------------------------------------------------------------------------
 * Name : NMI_Handler()
 *
 *
 * -------------------------------------------------------------------------- */
#if !defined(RTOS_FREERTOS)
void NMI_Handler(void)
{
}
#endif

/* --------------------------------------------------------------------------
 * Name : HardFault_Handler()
 *
 *
 * -------------------------------------------------------------------------- */
void HardFault_Handler(void)
{
   _Error_Handler(__FILE__, __LINE__);
}

/* --------------------------------------------------------------------------
 * Name : MemManage_Handler()
 *
 *
 * -------------------------------------------------------------------------- */
void MemManage_Handler(void)
{
   _Error_Handler(__FILE__, __LINE__);
}

/* --------------------------------------------------------------------------
 * Name : BusFault_Handler()
 *
 *
 * -------------------------------------------------------------------------- */
void BusFault_Handler(void)
{
   _Error_Handler(__FILE__, __LINE__);
}

/* --------------------------------------------------------------------------
 * Name : UsageFault_Handler()
 *
 *
 * -------------------------------------------------------------------------- */
void UsageFault_Handler(void)
{
   _Error_Handler(__FILE__, __LINE__);
}

/* --------------------------------------------------------------------------
 * Name : SVC_Handler()
 *
 *
 * -------------------------------------------------------------------------- */
#if !defined(RTOS_FREERTOS)
void SVC_Handler(void)
{
}
#endif

/* --------------------------------------------------------------------------
 * Name : DebugMon_Handler()
 *
 *
 * -------------------------------------------------------------------------- */
void DebugMon_Handler(void)
{
}

/* --------------------------------------------------------------------------
 * Name : PendSV_Handler()
 *
 *
 * -------------------------------------------------------------------------- */
#if !defined(RTOS_FREERTOS)
void PendSV_Handler(void)
{
}
#endif


#if defined(UART_DEBUG_OUTPUT)
__weak void debug_tick_timer_handler()
{
}
#endif

/* --------------------------------------------------------------------------
 * Name : SysTick_Handler()
 *
 *
 * -------------------------------------------------------------------------- */
void SysTick_Handler(void)
{
   HAL_IncTick();
   HAL_SYSTICK_IRQHandler();

#if defined(UART_DEBUG_OUTPUT)
   debug_tick_timer_handler();
#endif


#if defined(RTOS_FREERTOS)
   osSystickHandler();
#endif
}

/******************************************************************************/
/* STM32F7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f7xx.s).                    */
/******************************************************************************/

/* --------------------------------------------------------------------------
 * Name : BSP_LTDC_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
__weak void BSP_LTDC_IRQHandler(void)
{
}

/* --------------------------------------------------------------------------
 * Name : LTDC_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void LTDC_IRQHandler(void)
{
   BSP_LTDC_IRQHandler();
}

/* --------------------------------------------------------------------------
 * Name : BSP_53L0A1_LEFT_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
__weak void BSP_53L0A1_LEFT_IRQHandler(void)
{
}

/* --------------------------------------------------------------------------
 * Name : EXTI2_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void EXTI2_IRQHandler(void)
{
   if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_2) != 0)
   {
      BSP_53L0A1_LEFT_IRQHandler();
      HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
   }
}

/* --------------------------------------------------------------------------
 * Name : BSP_53L0A1_CENTER_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
__weak void BSP_53L0A1_CENTER_IRQHandler(void)
{
}

/* --------------------------------------------------------------------------
 * Name : BSP_53L0A1_RIGHT_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
__weak void BSP_53L0A1_RIGHT_IRQHandler(void)
{
}

/* --------------------------------------------------------------------------
 * Name : EXTI9_5_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void EXTI9_5_IRQHandler(void)
{
   if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_6) != 0)
   {
      BSP_53L0A1_RIGHT_IRQHandler();
      HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
   }

   if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_9) != 0)
   {
      BSP_53L0A1_CENTER_IRQHandler();
      HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
   }
}

/* --------------------------------------------------------------------------
 * Name : BSP_TOUCH_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
__weak void BSP_TOUCH_IRQHandler(void)
{
}

/* --------------------------------------------------------------------------
 * Name : EXTI15_10_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void EXTI15_10_IRQHandler(void)
{
   if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_12) != 0)
   {
      HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
   }

   if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13) != 0)
   {
      BSP_TOUCH_IRQHandler();
      HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
   }
}

/* --------------------------------------------------------------------------
 * Name : ETH_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void ETH_IRQHandler(void)
{
   HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_6);
//   HAL_ETH_IRQHandler(&heth);
}

/* --------------------------------------------------------------------------
 * Name : USB_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
__weak void USB_IRQHandler(void)
{
}

/* --------------------------------------------------------------------------
 * Name : OTG_HS_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void OTG_HS_IRQHandler(void)
{
   USB_IRQHandler();
}

#if defined(QSPI_FLASH_USE)
/* --------------------------------------------------------------------------
 * Name : qspi_flash_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
__weak void qspi_flash_IRQHandler()
{
}

/* --------------------------------------------------------------------------
 * Name : QUADSPI_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void QUADSPI_IRQHandler(void)
{
   qspi_flash_IRQHandler();
}
#endif


#if defined(TIMER_TEST)
/* --------------------------------------------------------------------------
 * Name : tim1_up_tim10_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
__weak void tim1_up_tim10_IRQHandler()
{
}

/* --------------------------------------------------------------------------
 * Name : TIM1_UP_TIM10_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void TIM1_UP_TIM10_IRQHandler(void)
{
   tim1_up_tim10_IRQHandler();
}
#endif

