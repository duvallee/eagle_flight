/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
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
 *        This function handles Non maskable interrupt.
 *
 * -------------------------------------------------------------------------- */
#if !defined(RTOS_FREERTOS)
void NMI_Handler(void)
{
}
#endif

/* --------------------------------------------------------------------------
 * Name : HardFault_Handler()
 *        This function handles Hard fault interrupt.
 *
 * -------------------------------------------------------------------------- */
void HardFault_Handler(void)
{
   _Error_Handler(__FILE__, __LINE__);
}

/* --------------------------------------------------------------------------
 * Name : MemManage_Handler()
 *        This function handles Memory management fault.
 *
 * -------------------------------------------------------------------------- */
void MemManage_Handler(void)
{
   _Error_Handler(__FILE__, __LINE__);
}

/* --------------------------------------------------------------------------
 * Name : BusFault_Handler()
 *        This function handles Pre-fetch fault, memory access fault.
 *
 * -------------------------------------------------------------------------- */
void BusFault_Handler(void)
{
   _Error_Handler(__FILE__, __LINE__);
}

/* --------------------------------------------------------------------------
 * Name : UsageFault_Handler()
 *        This function handles Undefined instruction or illegal state.
 *
 * -------------------------------------------------------------------------- */
void UsageFault_Handler(void)
{
   _Error_Handler(__FILE__, __LINE__);
}

/* --------------------------------------------------------------------------
 * Name : SVC_Handler()
 *        This function handles System service call via SWI instruction.
 *
 * -------------------------------------------------------------------------- */
#if !defined(RTOS_FREERTOS)
void SVC_Handler(void)
{
}
#endif

/* --------------------------------------------------------------------------
 * Name : DebugMon_Handler()
 *        This function handles Debug monitor.
 *
 * -------------------------------------------------------------------------- */
void DebugMon_Handler(void)
{
}

/* --------------------------------------------------------------------------
 * Name : PendSV_Handler()
 *        This function handles Pendable request for system service.
 *
 * -------------------------------------------------------------------------- */
#if !defined(RTOS_FREERTOS)
void PendSV_Handler(void)
{
}
#endif

#if defined(SUPPORT_DEBUG_OUTPUT)
/* --------------------------------------------------------------------------
 * Name : debug_tick_timer_handler()
 *
 *
 * -------------------------------------------------------------------------- */
__weak void debug_tick_timer_handler()
{
}
#endif

/* --------------------------------------------------------------------------
 * Name : SysTick_Handler()
 *        This function handles System tick timer.
 *
 * -------------------------------------------------------------------------- */
void SysTick_Handler(void)
{
   HAL_IncTick();
   HAL_SYSTICK_IRQHandler();

#if defined(SUPPORT_DEBUG_OUTPUT)
   debug_tick_timer_handler();
#endif

#if defined(RTOS_FREERTOS)
   osSystickHandler();
#endif
}

#if defined(USE_USB)
/* --------------------------------------------------------------------------
 * Name : USB_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
__weak void USB_IRQHandler()
{
}

#if defined(USE_USB_FS)
/* --------------------------------------------------------------------------
 * Name : OTG_FS_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void OTG_FS_IRQHandler(void)
#elif defined(USE_USB_HS)
/* --------------------------------------------------------------------------
 * Name : OTG_HS_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void OTG_HS_IRQHandler(void)
#endif
{
   USB_IRQHandler();
}
#endif


/* --------------------------------------------------------------------------
 * Name : BSP_TIM1_CC_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
__weak void BSP_TIM1_CC_IRQHandler()
{
}

/* --------------------------------------------------------------------------
 * Name : TIM1_CC_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void TIM1_CC_IRQHandler(void)
{
   BSP_TIM1_CC_IRQHandler();
}

/* --------------------------------------------------------------------------
 * Name : BSP_TIM1_UP_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
__weak void BSP_TIM1_UP_IRQHandler()
{
}

/* --------------------------------------------------------------------------
 * Name : TIM1_UP_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void TIM1_UP_IRQHandler(void)
{
   BSP_TIM1_UP_IRQHandler();
}


