/**
  ******************************************************************************
  * File Name          : stm32h7xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
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

extern void _Error_Handler(char *, int);

/* --------------------------------------------------------------------------
 * Name : HAL_MspInit()
 *
 *
 * -------------------------------------------------------------------------- */
void HAL_MspInit(void)
{
   __HAL_RCC_SYSCFG_CLK_ENABLE();

   HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

   // System interrupt init
   // MemoryManagement_IRQn interrupt configuration
   HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);

   // BusFault_IRQn interrupt configuration
   HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);

   // UsageFault_IRQn interrupt configuration
   HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);

   // SVCall_IRQn interrupt configuration
   HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);

   // DebugMonitor_IRQn interrupt configuration
   HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);

   // PendSV_IRQn interrupt configuration
   HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);

   // SysTick_IRQn interrupt configuration
   HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

#if defined(USE_USB)
/* --------------------------------------------------------------------------
 * Name : HAL_HCD_MspInit()
 *
 *
 * -------------------------------------------------------------------------- */
void HAL_PCD_MspInit(PCD_HandleTypeDef* hpcd)
{
   GPIO_InitTypeDef GPIO_InitStruct;

   if (hpcd->Instance == USB2_OTG_FS)
   {
      __HAL_RCC_GPIOA_CLK_ENABLE();

      // Configure DM DP Pins
      GPIO_InitStruct.Pin                                = (GPIO_PIN_11 | GPIO_PIN_12);
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF10_OTG1_FS;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

      // Configure VBUS Pin
      GPIO_InitStruct.Pin                                = GPIO_PIN_9;
      GPIO_InitStruct.Mode                               = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

      // Configure ID pin
      GPIO_InitStruct.Pin                                = GPIO_PIN_10;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_OD;
      GPIO_InitStruct.Pull                               = GPIO_PULLUP;
      GPIO_InitStruct.Alternate                          = GPIO_AF10_OTG1_FS;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

      // Enable USB FS Clocks
      __HAL_RCC_USB2_OTG_FS_CLK_ENABLE();

      // Set USBFS Interrupt to the lowest priority
      HAL_NVIC_SetPriority(OTG_FS_IRQn, 6, 0);

      // Enable USBFS Interrupt
      HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
   }
   else if (hpcd->Instance == USB1_OTG_HS)
   {
      // Configure USB HS GPIOs
      __GPIOA_CLK_ENABLE();
      __GPIOB_CLK_ENABLE();
      __GPIOC_CLK_ENABLE();
      __GPIOH_CLK_ENABLE();
      __GPIOI_CLK_ENABLE();

      // ULPI - CLK
      GPIO_InitStruct.Pin                                = GPIO_PIN_5;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF10_OTG2_HS;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

      // ULPI - D0
      GPIO_InitStruct.Pin                                = GPIO_PIN_3;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF10_OTG2_HS;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

      // ULPI - D1 D2 D3 D4 D5 D6 D7
      GPIO_InitStruct.Pin                                = GPIO_PIN_0      |
                                                           GPIO_PIN_1      |
                                                           GPIO_PIN_5      |
                                                           GPIO_PIN_10     |
                                                           GPIO_PIN_11     |
                                                           GPIO_PIN_12     |
                                                           GPIO_PIN_13;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Alternate                          = GPIO_AF10_OTG2_HS;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      // ULPI - STP
      GPIO_InitStruct.Pin                                = GPIO_PIN_0;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Alternate                          = GPIO_AF10_OTG2_HS;
      HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

      // ULPI - NXT
      GPIO_InitStruct.Pin                                = GPIO_PIN_4;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Alternate                          = GPIO_AF10_OTG2_HS;
      HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

      // ULPI - DIR
      GPIO_InitStruct.Pin                                = GPIO_PIN_11;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Alternate                          = GPIO_AF10_OTG2_HS;
      HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
      __HAL_RCC_USB1_OTG_HS_ULPI_CLK_ENABLE();

      // Enable USB HS Clocks
      __HAL_RCC_USB1_OTG_HS_CLK_ENABLE();

      // Set USBHS Interrupt priority
      HAL_NVIC_SetPriority(OTG_HS_IRQn, 1, 0);

      // Enable USBHS Interrupt
      HAL_NVIC_EnableIRQ(OTG_HS_IRQn);
   }
}


/* --------------------------------------------------------------------------
 * Name : HAL_PCD_MspDeInit()
 *
 *
 * -------------------------------------------------------------------------- */
void HAL_PCD_MspDeInit(PCD_HandleTypeDef * hpcd)
{
   if (hpcd->Instance == USB2_OTG_FS)
   {
      // Disable USB HS Clocks
      __HAL_RCC_USB2_OTG_FS_CLK_DISABLE();
   }
   else if (hpcd->Instance == USB1_OTG_HS)
   {
      // Disable USB HS Clocks
      __HAL_RCC_USB1_OTG_HS_CLK_DISABLE();
      __HAL_RCC_USB1_OTG_HS_ULPI_CLK_DISABLE();
   }
}
#endif

/* --------------------------------------------------------------------------
 * Name : HAL_UART_MspInit()
 *
 *
 * -------------------------------------------------------------------------- */
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
   GPIO_InitTypeDef GPIO_InitStruct;

   if (huart->Instance == USART6)
   {
      // Peripheral clock enable
      __HAL_RCC_USART6_CLK_ENABLE();
      __HAL_RCC_GPIOG_CLK_ENABLE();

      GPIO_InitStruct.Pin                                = UART_DEBUG_RX_PIN | UART_DEBUG_TX_PIN;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate                          = GPIO_AF7_USART6;
      HAL_GPIO_Init(UART_DEBUG_RX_GPIO_PORT, &GPIO_InitStruct);
   }
}

/* --------------------------------------------------------------------------
 * Name : HAL_UART_MspDeInit()
 *
 *
 * -------------------------------------------------------------------------- */
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
   if (huart->Instance == USART6)
   {
      // Peripheral clock disable
      __HAL_RCC_USART6_CLK_DISABLE();
   }
}

// ***************************************************************************
// Fuction      : HAL_TIM_Base_MspInit()
// Description  : 
// 
//
// ***************************************************************************
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
   GPIO_InitTypeDef GPIO_InitStruct;
   if (htim_base->Instance == TIM1)
   {
      // Peripheral clock enable
      __TIM1_CLK_ENABLE();
      __HAL_RCC_GPIOE_CLK_ENABLE();
  
      GPIO_InitStruct.Pin                                = GPIO_PIN_9 | GPIO_PIN_11;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_PULLDOWN;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF1_TIM1;
      HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
   }
}

// ***************************************************************************
// Fuction      : HAL_TIM_Base_MspDeInit()
// Description  : 
// 
//
// ***************************************************************************
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
   if (htim_base->Instance == TIM1)
   {
      __TIM1_CLK_DISABLE();
   }
}



