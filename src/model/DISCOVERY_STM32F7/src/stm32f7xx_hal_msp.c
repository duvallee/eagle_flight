/**
  ******************************************************************************
  * File Name          : stm32f7xx_hal_msp.c
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
 *        Initializes the Global MSP.
 *
 * -------------------------------------------------------------------------- */
void HAL_MspInit(void)
{
   HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

   /* System interrupt init*/
   /* MemoryManagement_IRQn interrupt configuration */
   HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
   /* BusFault_IRQn interrupt configuration */
   HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
   /* UsageFault_IRQn interrupt configuration */
   HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
   /* SVCall_IRQn interrupt configuration */
   HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
   /* DebugMonitor_IRQn interrupt configuration */
   HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
   /* PendSV_IRQn interrupt configuration */
   HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
   /* SysTick_IRQn interrupt configuration */
   HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* --------------------------------------------------------------------------
 * Name : HAL_FMC_MspInit()
 *        
 *
 * -------------------------------------------------------------------------- */
static uint32_t FMC_Initialized                          = 0;
static void HAL_FMC_MspInit(void)
{
//   static DMA_HandleTypeDef dma_handle;
   GPIO_InitTypeDef gpio_init_structure;

   if (FMC_Initialized)
   {
      return;
   }
   FMC_Initialized                                       = 1;

   /* Enable FMC clock */
   __HAL_RCC_FMC_CLK_ENABLE();

   /* Enable chosen DMAx clock */
//   __DMAx_CLK_ENABLE();

   /* Enable GPIOs clock */
   __HAL_RCC_GPIOC_CLK_ENABLE();
   __HAL_RCC_GPIOD_CLK_ENABLE();
   __HAL_RCC_GPIOE_CLK_ENABLE();
   __HAL_RCC_GPIOF_CLK_ENABLE();
   __HAL_RCC_GPIOG_CLK_ENABLE();
   __HAL_RCC_GPIOH_CLK_ENABLE();

   /* Common GPIO configuration */
   gpio_init_structure.Mode                              = GPIO_MODE_AF_PP;
   gpio_init_structure.Pull                              = GPIO_PULLUP;
   gpio_init_structure.Speed                             = GPIO_SPEED_FAST;
   gpio_init_structure.Alternate                         = GPIO_AF12_FMC;

   /* GPIOC configuration */
   gpio_init_structure.Pin                               = GPIO_PIN_3;
   HAL_GPIO_Init(GPIOC, &gpio_init_structure);

   /* GPIOD configuration */
   gpio_init_structure.Pin                               = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 |
                                                           GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
   HAL_GPIO_Init(GPIOD, &gpio_init_structure);

   /* GPIOE configuration */  
   gpio_init_structure.Pin                               = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 |
                                                           GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 |
                                                           GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
   HAL_GPIO_Init(GPIOE, &gpio_init_structure);

   /* GPIOF configuration */  
   gpio_init_structure.Pin                               = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                                                           GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 |
                                                           GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
   HAL_GPIO_Init(GPIOF, &gpio_init_structure);

   /* GPIOG configuration */  
   gpio_init_structure.Pin                               = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 |
                                                           GPIO_PIN_8 | GPIO_PIN_15;
   HAL_GPIO_Init(GPIOG, &gpio_init_structure);

   /* GPIOH configuration */  
   gpio_init_structure.Pin                               = GPIO_PIN_3 | GPIO_PIN_5;
   HAL_GPIO_Init(GPIOH, &gpio_init_structure); 

#if 0
   /* Configure common DMA parameters */
   dma_handle.Init.Channel             = SDRAM_DMAx_CHANNEL;
   dma_handle.Init.Direction           = DMA_MEMORY_TO_MEMORY;
   dma_handle.Init.PeriphInc           = DMA_PINC_ENABLE;
   dma_handle.Init.MemInc              = DMA_MINC_ENABLE;
   dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
   dma_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
   dma_handle.Init.Mode                = DMA_NORMAL;
   dma_handle.Init.Priority            = DMA_PRIORITY_HIGH;
   dma_handle.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
   dma_handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
   dma_handle.Init.MemBurst            = DMA_MBURST_SINGLE;
   dma_handle.Init.PeriphBurst         = DMA_PBURST_SINGLE; 

   dma_handle.Instance = SDRAM_DMAx_STREAM;

   /* Associate the DMA handle */
   __HAL_LINKDMA(hsdram, hdma, dma_handle);

   /* Deinitialize the stream for new transfer */
   HAL_DMA_DeInit(&dma_handle);

   /* Configure the DMA stream */
   HAL_DMA_Init(&dma_handle); 

   /* NVIC configuration for DMA transfer complete interrupt */
   HAL_NVIC_SetPriority(SDRAM_DMAx_IRQn, 0x0F, 0);
   HAL_NVIC_EnableIRQ(SDRAM_DMAx_IRQn);
#endif
}

/* --------------------------------------------------------------------------
 * Name : HAL_SDRAM_MspInit()
 *        
 *
 * -------------------------------------------------------------------------- */
void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef* hsdram)
{
   HAL_FMC_MspInit();
}

/* --------------------------------------------------------------------------
 * Name : HAL_FMC_MspDeInit()
 *        
 *
 * -------------------------------------------------------------------------- */
static void HAL_FMC_MspDeInit(void)
{
   __HAL_RCC_FMC_CLK_DISABLE();
}

/* --------------------------------------------------------------------------
 * Name : HAL_SDRAM_MspDeInit()
 *        
 *
 * -------------------------------------------------------------------------- */
void HAL_SDRAM_MspDeInit(SDRAM_HandleTypeDef* hsdram)
{
   HAL_FMC_MspDeInit();
}

/* --------------------------------------------------------------------------
 * Name : HAL_LTDC_MspInit()
 *        
 *
 * -------------------------------------------------------------------------- */
void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
{
   GPIO_InitTypeDef gpio_init_structure;
   static RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

   /* Enable the LTDC clocks */
   __HAL_RCC_LTDC_CLK_ENABLE();

   /* LCD clock configuration */
   /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
   /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 429 Mhz */
   /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 429/5 = 85 Mhz */
   /* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_2 = 85/4 = 21 Mhz */
   PeriphClkInitStruct.PeriphClockSelection              = RCC_PERIPHCLK_LTDC;
   PeriphClkInitStruct.PLLSAI.PLLSAIN                    = 192;
   PeriphClkInitStruct.PLLSAI.PLLSAIR                    = 5;
   PeriphClkInitStruct.PLLSAIDivR                        = RCC_PLLSAIDIVR_4;
   HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);   

   /* Enable GPIOs clock */
   __HAL_RCC_GPIOE_CLK_ENABLE();
   __HAL_RCC_GPIOG_CLK_ENABLE();
   __HAL_RCC_GPIOI_CLK_ENABLE();
   __HAL_RCC_GPIOJ_CLK_ENABLE();
   __HAL_RCC_GPIOK_CLK_ENABLE();

   /*** LTDC Pins configuration ***/
   /* GPIOE configuration */
   gpio_init_structure.Pin                               = GPIO_PIN_4;
   gpio_init_structure.Mode                              = GPIO_MODE_AF_PP;
   gpio_init_structure.Pull                              = GPIO_NOPULL;
   gpio_init_structure.Speed                             = GPIO_SPEED_FAST;
   gpio_init_structure.Alternate                         = GPIO_AF14_LTDC;
   HAL_GPIO_Init(GPIOE, &gpio_init_structure);

   /* GPIOG configuration */
   gpio_init_structure.Pin                               = GPIO_PIN_12;
   gpio_init_structure.Mode                              = GPIO_MODE_AF_PP;
   gpio_init_structure.Alternate                         = GPIO_AF9_LTDC;
   HAL_GPIO_Init(GPIOG, &gpio_init_structure);

   /* GPIOI LTDC alternate configuration */
   gpio_init_structure.Pin                               = GPIO_PIN_9 | GPIO_PIN_10 | \
                                                           GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
   gpio_init_structure.Mode                              = GPIO_MODE_AF_PP;
   gpio_init_structure.Alternate                         = GPIO_AF14_LTDC;
   HAL_GPIO_Init(GPIOI, &gpio_init_structure);

   /* GPIOJ configuration */  
   gpio_init_structure.Pin                               = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
                                                           GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | \
                                                           GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
                                                           GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
   gpio_init_structure.Mode                              = GPIO_MODE_AF_PP;
   gpio_init_structure.Alternate                         = GPIO_AF14_LTDC;
   HAL_GPIO_Init(GPIOJ, &gpio_init_structure);  

   /* GPIOK configuration */  
   gpio_init_structure.Pin                               = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | \
                                                           GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
   gpio_init_structure.Mode                              = GPIO_MODE_AF_PP;
   gpio_init_structure.Alternate                         = GPIO_AF14_LTDC;
   HAL_GPIO_Init(GPIOK, &gpio_init_structure);

   /* LCD_DISP GPIO configuration */
   gpio_init_structure.Pin                               = GPIO_PIN_12;          /* LCD_DISP pin has to be manually controlled */
   gpio_init_structure.Mode                              = GPIO_MODE_OUTPUT_PP;
   HAL_GPIO_Init(GPIOI, &gpio_init_structure);

   /* LCD_BL_CTRL GPIO configuration */
   gpio_init_structure.Pin                               = GPIO_PIN_3;           /* LCD_BL_CTRL pin has to be manually controlled */
   gpio_init_structure.Mode                              = GPIO_MODE_OUTPUT_PP;
   HAL_GPIO_Init(GPIOK, &gpio_init_structure);

   /* Set LTDC Interrupt to the lowest priority */
   HAL_NVIC_SetPriority(LTDC_IRQn, 0xE, 0);

   /* Enable LTDC Interrupt */
   HAL_NVIC_EnableIRQ(LTDC_IRQn);
}

/* --------------------------------------------------------------------------
 * Name : HAL_LTDC_MspDeInit()
 *        
 *
 * -------------------------------------------------------------------------- */
void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef* hltdc)
{
   /* Reset peripherals */
   /* Enable LTDC reset state */
   __HAL_RCC_LTDC_FORCE_RESET();

   /* Release LTDC from reset state */ 
   __HAL_RCC_LTDC_RELEASE_RESET();
}

/* --------------------------------------------------------------------------
 * Name : HAL_DMA2D_MspInit()
 *        
 *
 * -------------------------------------------------------------------------- */
void HAL_DMA2D_MspInit(DMA2D_HandleTypeDef* hdma2d)
{
   if(hdma2d->Instance==DMA2D)
   {
      /* Peripheral clock enable */
      __HAL_RCC_DMA2D_CLK_ENABLE();
   }
}

/* --------------------------------------------------------------------------
 * Name : HAL_DMA2D_MspDeInit()
 *        
 *
 * -------------------------------------------------------------------------- */
void HAL_DMA2D_MspDeInit(DMA2D_HandleTypeDef* hdma2d)
{
   if(hdma2d->Instance==DMA2D)
   {
      /* Peripheral clock disable */
      __HAL_RCC_DMA2D_CLK_DISABLE();
   }
}

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
      /* Peripheral clock enable */
      __HAL_RCC_USART6_CLK_ENABLE();
      __HAL_RCC_GPIOC_CLK_ENABLE();

      /**USART6 GPIO Configuration    
      PC6     ------> USART6_TX
      PC7     ------> USART6_RX 
      */
      GPIO_InitStruct.Pin                                   = GPIO_PIN_6 | GPIO_PIN_7;
      GPIO_InitStruct.Mode                                  = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                                  = GPIO_PULLUP;
      GPIO_InitStruct.Speed                                 = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate                             = GPIO_AF8_USART6;
      HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
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
      /* Peripheral clock disable */
      __HAL_RCC_USART6_CLK_DISABLE();
   }
}

/* --------------------------------------------------------------------------
 * Name : HAL_I2C_MspInit()
 *        
 *
 * -------------------------------------------------------------------------- */
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
   GPIO_InitTypeDef GPIO_InitStruct;
   if (hi2c->Instance == I2C1)
   {
      // Peripheral clock enable
      __HAL_RCC_I2C1_CLK_ENABLE();
      __HAL_RCC_GPIOB_CLK_ENABLE();

      // I2C1 GPIO Configuration    
      // PB8     ------> I2C1_SCL
      // PB9     ------> I2C1_SDA 
      GPIO_InitStruct.Pin                                = P_NUCLEO_53L0A1_SCL_PIN | P_NUCLEO_53L0A1_SDA_PIN;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_OD;
      GPIO_InitStruct.Pull                               = GPIO_PULLUP;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF4_I2C1;
      HAL_GPIO_Init(P_NUCLEO_53L0A1_SCL_PORT, &GPIO_InitStruct);
   }
   else if (hi2c->Instance == I2C3)
   {
      // Peripheral clock enable
      __HAL_RCC_I2C3_CLK_ENABLE();
      __HAL_RCC_GPIOH_CLK_ENABLE();

      // I2C3 GPIO Configuration    
      // PH7     ------> I2C3_SCL
      // PH8     ------> I2C3_SDA 
      GPIO_InitStruct.Pin                                = TOUCH_FT5536_SCL_PIN | TOUCH_FT5536_SDA_PIN;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_OD;
      GPIO_InitStruct.Pull                               = GPIO_PULLUP;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF4_I2C3;
      HAL_GPIO_Init(TOUCH_FT5536_SCL_PORT, &GPIO_InitStruct);  
   }
}

/* --------------------------------------------------------------------------
 * Name : HAL_I2C_MspDeInit()
 *        
 *
 * -------------------------------------------------------------------------- */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
   if (hi2c->Instance == I2C3)
   {
      /* Peripheral clock disable */
      __HAL_RCC_I2C3_CLK_DISABLE();

      /**I2C3 GPIO Configuration    
      PH7     ------> I2C3_SCL
      PH8     ------> I2C3_SDA 
      */
      HAL_GPIO_DeInit(TOUCH_FT5536_SCL_PORT, TOUCH_FT5536_SCL_PIN | TOUCH_FT5536_SDA_PIN);
   }
}


/* --------------------------------------------------------------------------
 * Name : HAL_HCD_MspInit()
 *
 *
 * -------------------------------------------------------------------------- */
void HAL_PCD_MspInit(PCD_HandleTypeDef* hpcd)
{
   GPIO_InitTypeDef GPIO_InitStruct;

   if (hpcd->Instance == USB_OTG_HS)
   {
      // Configure USB HS GPIOs
      __GPIOA_CLK_ENABLE();
      __GPIOB_CLK_ENABLE();
      __GPIOC_CLK_ENABLE();
      __GPIOH_CLK_ENABLE();

      // ULPI - CLK, D0
      GPIO_InitStruct.Pin                                = GPIO_PIN_5      |
                                                           GPIO_PIN_3;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF10_OTG_HS;
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
      GPIO_InitStruct.Alternate                          = GPIO_AF10_OTG_HS;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      // ULPI - STP, DIR
      GPIO_InitStruct.Pin                                = GPIO_PIN_0      |
                                                           GPIO_PIN_2;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Alternate                          = GPIO_AF10_OTG_HS;
      HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

      // ULPI - NXT
      GPIO_InitStruct.Pin                                = GPIO_PIN_4;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Alternate                          = GPIO_AF10_OTG_HS;
      HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

      // Enable USB HS Clocks
      __HAL_RCC_USB_OTG_HS_CLK_ENABLE();
      __HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE();

      // Set USBHS Interrupt priority
      HAL_NVIC_SetPriority(OTG_HS_IRQn, 5, 0);

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
   if (hpcd->Instance == USB_OTG_HS)
   {
      /* Disable USB HS Clocks */
      __HAL_RCC_USB_OTG_HS_CLK_DISABLE();
      __HAL_RCC_USB_OTG_HS_ULPI_CLK_DISABLE();
   }
}

#if defined(NET_LWIP)
/* --------------------------------------------------------------------------
 * Name : HAL_ETH_MspInit()
 *
 *
 * -------------------------------------------------------------------------- */
void HAL_ETH_MspInit(ETH_HandleTypeDef* ethHandle)
{
   GPIO_InitTypeDef GPIO_InitStruct;

   if (ethHandle->Instance == ETH)
   {
      /** ETH GPIO Configuration    
      RMII_REF_CLK ----------------------> PA1
      RMII_MDIO -------------------------> PA2
      RMII_MDC --------------------------> PC1
      RMII_MII_CRS_DV -------------------> PA7
      RMII_MII_RXD0 ---------------------> PC4
      RMII_MII_RXD1 ---------------------> PC5
      RMII_MII_RXER ---------------------> PG2
      RMII_MII_TX_EN --------------------> PG11
      RMII_MII_TXD0 ---------------------> PG13
      RMII_MII_TXD1 ---------------------> PG14
      */

      GPIO_InitStruct.Pin                                = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF11_ETH;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

      GPIO_InitStruct.Pin                                = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF11_ETH;
      HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

      GPIO_InitStruct.Pin                                = GPIO_PIN_2 | GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF11_ETH;
      HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

      /* Enable the Ethernet global Interrupt */
      HAL_NVIC_SetPriority(ETH_IRQn, 0x7, 0);
      HAL_NVIC_EnableIRQ(ETH_IRQn);

      /* Enable Peripheral clock */
      __HAL_RCC_ETH_CLK_ENABLE();
   }
}

/* --------------------------------------------------------------------------
 * Name : HAL_ETH_MspDeInit()
 *
 *
 * -------------------------------------------------------------------------- */
void HAL_ETH_MspDeInit(ETH_HandleTypeDef* ethHandle)
{
   if (ethHandle->Instance == ETH)
   {
      /* Peripheral clock disable */
      __HAL_RCC_ETH_CLK_DISABLE();

      /** ETH GPIO Configuration    
      RMII_REF_CLK ----------------------> PA1
      RMII_MDIO -------------------------> PA2
      RMII_MDC --------------------------> PC1
      RMII_MII_CRS_DV -------------------> PA7
      RMII_MII_RXD0 ---------------------> PC4
      RMII_MII_RXD1 ---------------------> PC5
      RMII_MII_RXER ---------------------> PG2
      RMII_MII_TX_EN --------------------> PG11
      RMII_MII_TXD0 ---------------------> PG13
      RMII_MII_TXD1 ---------------------> PG14
      */
      HAL_GPIO_DeInit(GPIOG, GPIO_PIN_14 | GPIO_PIN_13 | GPIO_PIN_11 | GPIO_PIN_2);
      HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1  | GPIO_PIN_4  | GPIO_PIN_5);
      HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_7);

      /* Peripheral interrupt Deinit*/
      HAL_NVIC_DisableIRQ(ETH_IRQn);
   }
}
#endif

#if defined(QSPI_FLASH_USE)
/* --------------------------------------------------------------------------
 * Name : HAL_QSPI_MspInit()
 *
 *
 * -------------------------------------------------------------------------- */
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
   GPIO_InitTypeDef gpio_init_structure;

   // ##-1- Enable peripherals and GPIO Clocks #################################
   // Enable the QuadSPI memory interface clock
   __HAL_RCC_QSPI_CLK_ENABLE();

   // Reset the QuadSPI memory interface
   __HAL_RCC_QSPI_FORCE_RESET();
   __HAL_RCC_QSPI_RELEASE_RESET();

   // Enable GPIO clocks
   __HAL_RCC_GPIOB_CLK_ENABLE();
   __HAL_RCC_GPIOD_CLK_ENABLE();
   __HAL_RCC_GPIOE_CLK_ENABLE();

   // ##-2- Configure peripheral GPIO ##########################################
   // QSPI CS GPIO pin configuration
   gpio_init_structure.Pin                               = GPIO_PIN_6;
   gpio_init_structure.Mode                              = GPIO_MODE_AF_PP;
   gpio_init_structure.Pull                              = GPIO_PULLUP;
   gpio_init_structure.Speed                             = GPIO_SPEED_HIGH;
   gpio_init_structure.Alternate                         = GPIO_AF10_QUADSPI;
   HAL_GPIO_Init(GPIOB, &gpio_init_structure);

   // QSPI CLK GPIO pin configuration
   gpio_init_structure.Pin                               = GPIO_PIN_2;
   gpio_init_structure.Pull                              = GPIO_NOPULL;
   gpio_init_structure.Alternate                         = GPIO_AF9_QUADSPI;
   HAL_GPIO_Init(GPIOB, &gpio_init_structure);

   // QSPI D0 GPIO pin configuration
   gpio_init_structure.Pin                               = GPIO_PIN_11;
   gpio_init_structure.Alternate                         = GPIO_AF9_QUADSPI;
   HAL_GPIO_Init(GPIOD, &gpio_init_structure);

   // QSPI D1 GPIO pin configuration
   gpio_init_structure.Pin                               = GPIO_PIN_12;
   gpio_init_structure.Alternate                         = GPIO_AF9_QUADSPI;
   HAL_GPIO_Init(GPIOD, &gpio_init_structure);

   // QSPI D2 GPIO pin configuration
   gpio_init_structure.Pin                               = GPIO_PIN_2;
   gpio_init_structure.Alternate                         = GPIO_AF9_QUADSPI;
   HAL_GPIO_Init(GPIOE, &gpio_init_structure);

   // QSPI D3 GPIO pin configuration
   gpio_init_structure.Pin                               = GPIO_PIN_13;
   gpio_init_structure.Alternate                         = GPIO_AF9_QUADSPI;
   HAL_GPIO_Init(GPIOD, &gpio_init_structure);

   // ##-3- Configure the NVIC for QSPI #########################################
   // NVIC configuration for QSPI interrupt
   HAL_NVIC_SetPriority(QUADSPI_IRQn, 0x0F, 0);
   HAL_NVIC_EnableIRQ(QUADSPI_IRQn);
}

/* --------------------------------------------------------------------------
 * Name : HAL_QSPI_MspInit()
 *
 *
 * -------------------------------------------------------------------------- */
void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef *hqspi)
{
   // ##-1- Disable the NVIC for QSPI ###########################################
   HAL_NVIC_DisableIRQ(QUADSPI_IRQn);

   // ##-2- Disable peripherals and GPIO Clocks ################################
   // De-Configure QSPI pins
   HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
   HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2);
   HAL_GPIO_DeInit(GPIOD, GPIO_PIN_11);
   HAL_GPIO_DeInit(GPIOD, GPIO_PIN_12);
   HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2);
   HAL_GPIO_DeInit(GPIOD, GPIO_PIN_13);

   // ##-3- Reset peripherals ##################################################
   // Reset the QuadSPI memory interface
   __HAL_RCC_QSPI_FORCE_RESET();
   __HAL_RCC_QSPI_RELEASE_RESET();

   /* Disable the QuadSPI memory interface clock */
   __HAL_RCC_QSPI_CLK_DISABLE();
}

#endif

#if defined(TIMER_TEST)
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
      __HAL_RCC_GPIOA_CLK_ENABLE();
  
      GPIO_InitStruct.Pin                                = ARDUINO_CN7_PIN_03;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate                          = GPIO_AF1_TIM1;
      HAL_GPIO_Init(ARDUINO_CN7_PIN_03_PORT, &GPIO_InitStruct);
   }
   else if (htim_base->Instance == TIM12)
   {
      // Peripheral clock enable
      __TIM12_CLK_ENABLE();
      __HAL_RCC_GPIOB_CLK_ENABLE();
      __HAL_RCC_GPIOH_CLK_ENABLE();

      // Connector 7 - Pin 4 : (RTC_REFIN, TIM1_CH3N, TIM8_CH_3N, SPI2_MOSI/I2S2_SD, TIM12_CH2 ...)
      // 5V tolerant I/O
      GPIO_InitStruct.Pin                                = ARDUINO_CN7_PIN_04;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_FREQ_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF9_TIM12;
      HAL_GPIO_Init(ARDUINO_CN7_PIN_04_PORT, &GPIO_InitStruct);                     // TIM12_CH2

      // Connector 4 - Pin 7 : (I2C2_SMBA, SPI5_SCK,TIM12_CH1 ...)
      // 5V tolerant I/O
      GPIO_InitStruct.Pin                                = ARDUINO_CN4_PIN_07;
      GPIO_InitStruct.Mode                               = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull                               = GPIO_NOPULL;
      GPIO_InitStruct.Speed                              = GPIO_SPEED_FREQ_HIGH;
      GPIO_InitStruct.Alternate                          = GPIO_AF9_TIM12;
      HAL_GPIO_Init(ARDUINO_CN4_PIN_07_PORT, &GPIO_InitStruct);                     // TIM12_CH1
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

#endif


