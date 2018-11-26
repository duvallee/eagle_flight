/*
 * File: blue_nrg_spi1.c
 *
 * Written by duvallee in 2018
 *
 */
#include "main.h"
#include "blue_nrg_spi1.h"

// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
static SPI_HandleTypeDef g_spi1_blueng_Handle;


// SPBTLE-RF


/* --------------------------------------------------------------------------
 * Name : BLUE_NRG_EXTI_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void BLUE_NRG_EXTI_IRQHandler()
{
//         HCI_Isr();
//         HCI_ProcessEvent                                = 1;
}


// ***************************************************************************
// Fuction      : BLUE_NRG_SPI1_Init()
// Description  : 
// 
//
// ***************************************************************************
void BLUE_NRG_SPI1_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct;
   __GPIOB_CLK_ENABLE();

   // Reset
   GPIO_InitStruct.Pin                                   = GPIO_PIN_2;
   GPIO_InitStruct.Mode                                  = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull                                  = GPIO_PULLUP;
   GPIO_InitStruct.Speed                                 = GPIO_SPEED_LOW;
   GPIO_InitStruct.Alternate                             = 0;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);	                        /*Added to avoid spurious interrupt from the BlueNRG */

   // NSS/CSN/CS
   GPIO_InitStruct.Pin                                   = GPIO_PIN_0;
   GPIO_InitStruct.Mode                                  = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull                                  = GPIO_PULLUP;
   GPIO_InitStruct.Speed                                 = GPIO_SPEED_HIGH;
   GPIO_InitStruct.Alternate                             = 0;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);


   g_spi1_blueng_Handle.Instance                         = SPI1;
   g_spi1_blueng_Handle.Init.Mode                        = SPI_MODE_MASTER;
   g_spi1_blueng_Handle.Init.Direction                   = SPI_DIRECTION_2LINES;
   g_spi1_blueng_Handle.Init.DataSize                    = SPI_DATASIZE_8BIT;
   g_spi1_blueng_Handle.Init.CLKPolarity                 = SPI_POLARITY_LOW;
   g_spi1_blueng_Handle.Init.CLKPhase                    = SPI_PHASE_1EDGE;
   g_spi1_blueng_Handle.Init.NSS                         = SPI_NSS_SOFT;
   g_spi1_blueng_Handle.Init.FirstBit                    = SPI_FIRSTBIT_MSB;
   g_spi1_blueng_Handle.Init.TIMode                      = SPI_TIMODE_DISABLED;
   g_spi1_blueng_Handle.Init.CRCPolynomial               = 7;
   g_spi1_blueng_Handle.Init.BaudRatePrescaler           = SPI_BAUDRATEPRESCALER_16;
   g_spi1_blueng_Handle.Init.CRCCalculation              = SPI_CRCCALCULATION_DISABLED;

   if (HAL_SPI_Init(&g_spi1_blueng_Handle) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   // IRQ -- INPUT
   GPIO_InitStruct.Pin                                   = GPIO_PIN_4;
   GPIO_InitStruct.Mode                                  = GPIO_MODE_IT_RISING;
   GPIO_InitStruct.Pull                                  = GPIO_NOPULL;
   GPIO_InitStruct.Speed                                 = GPIO_SPEED_HIGH;
   GPIO_InitStruct.Alternate                             = 0;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   // Configure the NVIC for SPI
   HAL_NVIC_SetPriority(EXTI4_IRQn, 3, 0);    
   HAL_NVIC_EnableIRQ(EXTI4_IRQn);  

}



