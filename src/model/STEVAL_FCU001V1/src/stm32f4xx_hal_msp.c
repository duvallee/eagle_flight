/**
  ******************************************************************************
  * File Name          : stm32f4xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "battery_gauge.h"
#include "motor.h"

// ***************************************************************************
// Fuction      : HAL_MspInit()
// Description  : 
// 
//
// ***************************************************************************
void HAL_MspInit(void)
{
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


// ***************************************************************************
// Fuction      : HAL_UART_MspInit()
// Description  : 
// 
//
// ***************************************************************************
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
   GPIO_InitTypeDef GPIO_InitStruct;
   if (huart->Instance == USART1)
   {
      __HAL_RCC_USART1_CLK_ENABLE();
      // Peripheral clock enable
      __HAL_RCC_GPIOA_CLK_ENABLE();

      GPIO_InitStruct.Pin                                = UART_DEBUG_RX_PIN | UART_DEBUG_TX_PIN;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate                          = GPIO_AF7_USART2;
      HAL_GPIO_Init(UART_DEBUG_RX_GPIO_PORT, &GPIO_InitStruct);
   }
}

// ***************************************************************************
// Fuction      : HAL_ADC_MspInit()
// Description  : 
// 
//
// ***************************************************************************
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
   GPIO_InitTypeDef GPIO_InitStruct;
   if (hadc->Instance == ADC1)
   {
      __HAL_RCC_ADC1_CLK_ENABLE();

      // ADC1 GPIO Configuration
      // PB1     ------> ADC1_IN9 
      GPIO_InitStruct.Pin                                = VBAT_SENSE_GPIO_B_1;
      GPIO_InitStruct.Mode                               = GPIO_MODE_ANALOG;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      HAL_GPIO_Init(VBAT_SENSE_PORT, &GPIO_InitStruct);
  }
}

// ***************************************************************************
// Fuction      : HAL_ADC_MspDeInit()
// Description  : 
// 
//
// ***************************************************************************
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
   if (hadc->Instance == ADC1)
   {
      __HAL_RCC_ADC1_CLK_DISABLE();

      // ADC1 GPIO Configuration    
      // PB1     ------> ADC1_IN9 
      HAL_GPIO_DeInit(VBAT_SENSE_PORT, VBAT_SENSE_GPIO_B_1);
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
   if (htim_base->Instance == TIM2)
   {
      // Peripheral clock enable
      __TIM2_CLK_ENABLE();
      __HAL_RCC_GPIOA_CLK_ENABLE();
  
      // TIM2 GPIO Configuration    
      // PA0-WKUP       ------> TIM2_CH1
      // PA1            ------> TIM2_CH2
      // PA2            ------> TIM2_CH3
      // PA3            ------> TIM2_CH4 
      GPIO_InitStruct.Pin                                = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_PULLDOWN;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_FAST;
      GPIO_InitStruct.Alternate                          = GPIO_AF1_TIM2;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

#if defined(TIM2_REMOTE_CONTROL)
      // TIM2 interrupt init
      HAL_NVIC_SetPriority(TIM2_IRQn, 2, 0);
      HAL_NVIC_EnableIRQ(TIM2_IRQn);
#endif   // TIM2_REMOTE_CONTROL
   }
   else if(htim_base->Instance == TIM4)
   {
      // Peripheral clock enable
      __TIM4_CLK_ENABLE();
      __HAL_RCC_GPIOB_CLK_ENABLE();

      // TIM4 GPIO Configuration
      // PB6     ------> TIM4_CH1
      // PB7     ------> TIM4_CH2
      // PB8     ------> TIM4_CH3
      // PB9     ------> TIM4_CH4 

      GPIO_InitStruct.Pin                                = MOTOR_1_GPIO_B_6 | MOTOR_2_GPIO_B_7 | MOTOR_3_GPIO_B_8 | MOTOR_4_GPIO_B_9;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_LOW;
      GPIO_InitStruct.Alternate                          = GPIO_AF2_TIM4;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
   }
  else if(htim_base->Instance == TIM9)
  {
      // Peripheral clock enable
      __TIM9_CLK_ENABLE();

      // Peripheral interrupt init
      HAL_NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 3, 0);
      HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
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
   if (htim_base->Instance == TIM2)
   {
      __TIM2_CLK_DISABLE();

      // TIM2 GPIO Configuration    
      // PA0-WKUP       ------> TIM2_CH1
      // PA1            ------> TIM2_CH2
      // PA2            ------> TIM2_CH3
      // PA3            ------> TIM2_CH4 
      HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

#if defined(TIM2_REMOTE_CONTROL)
      // Peripheral interrupt DeInit
      HAL_NVIC_DisableIRQ(TIM2_IRQn);
#endif
   }
   else if (htim_base->Instance == TIM4)
   {
      // Peripheral clock disable
      __TIM4_CLK_DISABLE();

      // TIM4 GPIO Configuration    
      // PB6     ------> TIM4_CH1
      // PB7     ------> TIM4_CH2
      // PB8     ------> TIM4_CH3
      // PB9     ------> TIM4_CH4 
      HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);
   }
   else if (htim_base->Instance == TIM9)
   {
      // Peripheral clock disable
      __TIM9_CLK_DISABLE();

      // Peripheral interrupt DeInit
      HAL_NVIC_DisableIRQ(TIM1_BRK_TIM9_IRQn);
   }

}


// ***************************************************************************
// Fuction      : HAL_UART_MspDeInit()
// Description  : 
// 
//
// ***************************************************************************
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
   if (huart->Instance == USART1)
   {
      __HAL_RCC_USART1_CLK_DISABLE();

      // USART1 GPIO Configuration
      // PA9     ------> USART1_TX
      // PA10     ------> USART1_RX 
      HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);

      HAL_NVIC_DisableIRQ(USART1_IRQn);
   }
}

// ***************************************************************************
// Fuction      : HAL_PCD_MspInit()
// Description  : 
// 
//
// ***************************************************************************
void HAL_PCD_MspInit(PCD_HandleTypeDef* pcdHandle)
{
   GPIO_InitTypeDef GPIO_InitStruct;
   if (pcdHandle->Instance == USB_OTG_FS)
   {
      __HAL_RCC_GPIOA_CLK_ENABLE();
      GPIO_InitStruct.Pin                                = GPIO_PIN_11 | GPIO_PIN_12;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF10_OTG_FS;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

      // Peripheral clock enable
      __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

      /* Peripheral interrupt init */
      HAL_NVIC_SetPriority(OTG_FS_IRQn, 7, 0);
      HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
   }
  
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef* pcdHandle)
{
   if (pcdHandle->Instance == USB_OTG_FS)
   {
      // Peripheral clock disable
      __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
      HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11 | GPIO_PIN_12);

      // Peripheral interrupt Deinit
      HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
   }
}


// ***************************************************************************
// Fuction      : HAL_SPI_MspInit()
// Description  : 
// 
//
// ***************************************************************************
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
   GPIO_InitTypeDef GPIO_InitStruct;
   if (hspi->Instance == SPI1)
   {
      __GPIOA_CLK_ENABLE();
      __SPI1_CLK_ENABLE();

      // SCLK
      GPIO_InitStruct.Pin                                = GPIO_PIN_5;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_PULLDOWN;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF5_SPI1;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 

      // MISO
      GPIO_InitStruct.Pin                                = GPIO_PIN_6;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF5_SPI1;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

      // MOSI
      GPIO_InitStruct.Pin                                = GPIO_PIN_7;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF5_SPI1;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   }
   else if (hspi->Instance == SPI2)
   {
      __GPIOB_CLK_ENABLE();
      __SPI2_CLK_ENABLE();

      // SCLK
      GPIO_InitStruct.Pin                                = GPIO_PIN_13;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF5_SPI2;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 

      // MOSI
      GPIO_InitStruct.Pin                                = GPIO_PIN_15;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF5_SPI2;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

   }
}


// ***************************************************************************
// Fuction      : HAL_SPI_MspDeInit()
// Description  : 
// 
//
// ***************************************************************************
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
   if (hspi->Instance == SPI1)
   {
      __SPI1_CLK_DISABLE();
      HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
      HAL_NVIC_DisableIRQ(SPI1_IRQn);
   }
   else if (hspi->Instance == SPI2)
   {
      HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
      HAL_NVIC_DisableIRQ(SPI2_IRQn);
   }
}



