/*
 * File: sensor_spi2.c
 *
 * Written by duvallee in 2018
 *
 */
#include "main.h"
#include "sensor_spi2.h"

// ---------------------------------------------------------------------------
static SPI_HandleTypeDef g_spi1_sensor_Handle            =
{
   .Instance                                             = NULL,
};


static SPI_HandleTypeDef g_spi2_sensor_Handle            =
{
   .Instance                                             = NULL,
};

static SPI_HandleTypeDef g_spi3_sensor_Handle            =
{
   .Instance                                             = NULL,
};


#if 0
// ***************************************************************************
// Fuction      : spi_read()
// Description  : 
// 
//
// ***************************************************************************
static void spi_read(SPI_HandleTypeDef* pSpi, uint8_t *val)
{
   __disable_irq();
   __HAL_SPI_ENABLE(pSpi);
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __HAL_SPI_DISABLE(pSpi);
   __enable_irq();

   while ((pSpi->Instance->SR & SPI_FLAG_RXNE) != SPI_FLAG_RXNE)
   {
      ;
   }

   // read the received data
   *val                                                  = *(__IO uint8_t *) &(pSpi->Instance->DR);

   while ((pSpi->Instance->SR & SPI_FLAG_BSY) == SPI_FLAG_BSY)
   {
      ;
   }
}
#endif

// ***************************************************************************
// Fuction      : Sensor_SPI2_Read()
// Description  : 
// 
//
// ***************************************************************************
uint8_t Sensor_SPI2_Read(void* spi, void* port, uint32_t pin, uint8_t* pBuffer, uint16_t nbytes)
{
   HAL_GPIO_WritePin((GPIO_TypeDef *) port, pin, GPIO_PIN_RESET);

   // Interrupts should be disabled during this operation
   __disable_irq();
   __HAL_SPI_ENABLE(((SPI_HandleTypeDef *) spi));
   SPI_1LINE_RX(((SPI_HandleTypeDef *) spi));

   // Transfer loop
   while (nbytes > 1U)
   {
      // Check the RXNE flag
      if (((SPI_HandleTypeDef *) spi)->Instance->SR & SPI_FLAG_RXNE)
      {
         // read the received data
         *pBuffer                                     = *(__IO uint8_t *) &(((SPI_HandleTypeDef *) spi)->Instance->DR);
         pBuffer                                      += sizeof(uint8_t);
         nbytes--;
      }
   }

   /* In master RX mode the clock is automaticaly generated on the SPI enable.
   So to guarantee the clock generation for only one data, the clock must be
   disabled after the first bit and before the latest bit of the last Byte received */
   /* __DSB instruction are inserted to garantee that clock is Disabled in the right timeframe */

   __DSB();
   __DSB();
   __HAL_SPI_DISABLE(((SPI_HandleTypeDef *) spi));
   // Change the data line to output and enable the SPI
   SPI_1LINE_TX(((SPI_HandleTypeDef *) spi));

   __enable_irq();
  
   while ((((SPI_HandleTypeDef *) spi)->Instance->SR & SPI_FLAG_RXNE) != SPI_FLAG_RXNE)
   {
      ;
   }

   // read the received data
   *pBuffer                                              = *(__IO uint8_t *) &(((SPI_HandleTypeDef *) spi)->Instance->DR);
   while ((((SPI_HandleTypeDef *) spi)->Instance->SR & SPI_FLAG_BSY) == SPI_FLAG_BSY)
   {
      ;
   }
   HAL_GPIO_WritePin((GPIO_TypeDef *) port, pin, GPIO_PIN_SET);
   return 0;
}

// ***************************************************************************
// Fuction      : Sensor_SPI2_Write()
// Description  : 
// 
//
// ***************************************************************************
uint8_t Sensor_SPI2_Write(void* spi, void* port, uint32_t pin, uint8_t* pBuffer, uint16_t nbytes)
{
   HAL_GPIO_WritePin((GPIO_TypeDef *) port, pin, GPIO_PIN_RESET);

   __disable_irq();
   __HAL_SPI_ENABLE(((SPI_HandleTypeDef *) spi));
   SPI_1LINE_TX(((SPI_HandleTypeDef *) spi));

   // check TXE flag
   while ((((SPI_HandleTypeDef *) spi)->Instance->SR & SPI_FLAG_TXE) != SPI_FLAG_TXE)
   {
      ;
   }

   // Write the data
   *((__IO uint8_t*) &(((SPI_HandleTypeDef *) spi)->Instance->DR)) = *(pBuffer);

   // Wait BSY flag
   while ((((SPI_HandleTypeDef *) spi)->Instance->SR & SPI_SR_TXE) != SPI_SR_TXE)
   {
      ;
   }
   while ((((SPI_HandleTypeDef *) spi)->Instance->SR & SPI_FLAG_BSY) == SPI_FLAG_BSY)
   {
      ;
   }
   HAL_GPIO_WritePin((GPIO_TypeDef *) port, pin, GPIO_PIN_SET);
   __enable_irq();

   __HAL_SPI_DISABLE(((SPI_HandleTypeDef *) spi));
   return 0;
}


// ***************************************************************************
// Fuction      : Sensor_SPI2_Init()
// Description  : 
// 
//
// ***************************************************************************
void* Sensor_SPI2_Init(GPIO_TypeDef* port, uint32_t pin, SPI_TypeDef* spi)
{
   GPIO_InitTypeDef GPIO_InitStruct;

   if (GPIOA == port)
   {
      __GPIOA_CLK_ENABLE();
   }
   else if (GPIOB == port)
   {
      __GPIOB_CLK_ENABLE();
   }
   else if (GPIOC == port)
   {
      __GPIOC_CLK_ENABLE();
   }
   else if (GPIOD == port)
   {
      __GPIOD_CLK_ENABLE();
   }
   else if (GPIOE == port)
   {
      __GPIOE_CLK_ENABLE();
   }
   else if (GPIOH == port)
   {
      __GPIOH_CLK_ENABLE();
   }
   else
   {
      debug_output_error("Unknown GPIO Port \r\n");
      _Error_Handler(__FILE__, __LINE__);
   }

   // Chip Select Pin
   HAL_GPIO_WritePin((GPIO_TypeDef *) port, pin, GPIO_PIN_SET);

   GPIO_InitStruct.Pin                                   = pin;
   GPIO_InitStruct.Mode                                  = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull                                  = GPIO_NOPULL;
   GPIO_InitStruct.Speed                                 = GPIO_SPEED_HIGH;
   GPIO_InitStruct.Alternate                             = 0;
   HAL_GPIO_Init((GPIO_TypeDef *) port, &GPIO_InitStruct);
   HAL_GPIO_WritePin((GPIO_TypeDef *) port, pin, GPIO_PIN_SET);

   if (spi == SPI1)
   {
      if (g_spi1_sensor_Handle.Instance != NULL)
      {
         return ((void*) &g_spi1_sensor_Handle);
      }
      g_spi1_sensor_Handle.Instance                         = SPI1;
      g_spi1_sensor_Handle.Init.Mode                        = SPI_MODE_MASTER;
      g_spi1_sensor_Handle.Init.Direction                   = SPI_DIRECTION_1LINE;
      g_spi1_sensor_Handle.Init.DataSize                    = SPI_DATASIZE_8BIT;
      g_spi1_sensor_Handle.Init.CLKPolarity                 = SPI_POLARITY_HIGH;
      g_spi1_sensor_Handle.Init.CLKPhase                    = SPI_PHASE_2EDGE;
      g_spi1_sensor_Handle.Init.NSS                         = SPI_NSS_SOFT;
      g_spi1_sensor_Handle.Init.FirstBit                    = SPI_FIRSTBIT_MSB;
      g_spi1_sensor_Handle.Init.TIMode                      = SPI_TIMODE_DISABLED;
      g_spi1_sensor_Handle.Init.CRCPolynomial               = 7;
      g_spi1_sensor_Handle.Init.BaudRatePrescaler           = SPI_BAUDRATEPRESCALER_16;
      g_spi1_sensor_Handle.Init.CRCCalculation              = SPI_CRCCALCULATION_DISABLED;

      if (HAL_SPI_Init(&g_spi1_sensor_Handle) != HAL_OK)
      {
         _Error_Handler(__FILE__, __LINE__);
      }

      SPI_1LINE_TX(&g_spi1_sensor_Handle);
      __HAL_SPI_ENABLE(&g_spi1_sensor_Handle);

      return ((void*) &g_spi1_sensor_Handle);
   }
   else if (spi == SPI2)
   {
      if (g_spi2_sensor_Handle.Instance != NULL)
      {
         return ((void*) &g_spi2_sensor_Handle);
      }
      g_spi2_sensor_Handle.Instance                         = SPI2;
      g_spi2_sensor_Handle.Init.Mode                        = SPI_MODE_MASTER;
      g_spi2_sensor_Handle.Init.Direction                   = SPI_DIRECTION_1LINE;
      g_spi2_sensor_Handle.Init.DataSize                    = SPI_DATASIZE_8BIT;
      g_spi2_sensor_Handle.Init.CLKPolarity                 = SPI_POLARITY_HIGH;
      g_spi2_sensor_Handle.Init.CLKPhase                    = SPI_PHASE_2EDGE;
      g_spi2_sensor_Handle.Init.NSS                         = SPI_NSS_SOFT;
      g_spi2_sensor_Handle.Init.FirstBit                    = SPI_FIRSTBIT_MSB;
      g_spi2_sensor_Handle.Init.TIMode                      = SPI_TIMODE_DISABLED;
      g_spi2_sensor_Handle.Init.CRCPolynomial               = 7;
      g_spi2_sensor_Handle.Init.BaudRatePrescaler           = SPI_BAUDRATEPRESCALER_16;
      g_spi2_sensor_Handle.Init.CRCCalculation              = SPI_CRCCALCULATION_DISABLED;

      if (HAL_SPI_Init(&g_spi2_sensor_Handle) != HAL_OK)
      {
         _Error_Handler(__FILE__, __LINE__);
      }

      SPI_1LINE_TX(&g_spi2_sensor_Handle);
      __HAL_SPI_ENABLE(&g_spi2_sensor_Handle);

      return ((void*) &g_spi2_sensor_Handle);
   }
   else if (spi == SPI3)
   {
      if (g_spi3_sensor_Handle.Instance != NULL)
      {
         return ((void*) &g_spi3_sensor_Handle);
      }
      g_spi3_sensor_Handle.Instance                         = SPI3;
      g_spi3_sensor_Handle.Init.Mode                        = SPI_MODE_MASTER;
      g_spi3_sensor_Handle.Init.Direction                   = SPI_DIRECTION_1LINE;
      g_spi3_sensor_Handle.Init.DataSize                    = SPI_DATASIZE_8BIT;
      g_spi3_sensor_Handle.Init.CLKPolarity                 = SPI_POLARITY_HIGH;
      g_spi3_sensor_Handle.Init.CLKPhase                    = SPI_PHASE_2EDGE;
      g_spi3_sensor_Handle.Init.NSS                         = SPI_NSS_SOFT;
      g_spi3_sensor_Handle.Init.FirstBit                    = SPI_FIRSTBIT_MSB;
      g_spi3_sensor_Handle.Init.TIMode                      = SPI_TIMODE_DISABLED;
      g_spi3_sensor_Handle.Init.CRCPolynomial               = 7;
      g_spi3_sensor_Handle.Init.BaudRatePrescaler           = SPI_BAUDRATEPRESCALER_16;
      g_spi3_sensor_Handle.Init.CRCCalculation              = SPI_CRCCALCULATION_DISABLED;

      if (HAL_SPI_Init(&g_spi3_sensor_Handle) != HAL_OK)
      {
         _Error_Handler(__FILE__, __LINE__);
      }

      SPI_1LINE_TX(&g_spi3_sensor_Handle);
      __HAL_SPI_ENABLE(&g_spi3_sensor_Handle);

      return ((void*) &g_spi3_sensor_Handle);
   }
   else
   {
      debug_output_error("Unknown SPI Port \r\n");
      _Error_Handler(__FILE__, __LINE__);
   }

   return ((void*) NULL);
}




#if 0

// ---------------------------------------------------------------------------

// --------------------------------------------------------------------------------
enum SPI2_CHIP_CS
{
   SPI2_CHIP_UNSELECT                                    = 0,
   SPI2_CHIP_SELECT                                      = 1,
};

// LSM6DSL A+GIMU            LSM6DS33_CS (PA8)
// LIS2MDL Magnetometer      LIS2MDL_CS (PB2)
// LPS22HD Pressure Sensor   LPS22HB_CS (PC13)

// ***************************************************************************
// Fuction      : Chip_Select_LSM6DSL()
// Description  : 
// 
//
// ***************************************************************************
static void Chip_Select_LSM6DSL(enum SPI2_CHIP_CS cs)
{
   (cs == SPI2_CHIP_SELECT) ? HAL_GPIO_WritePin(LSM6DSL_SPI2_CS_PORT, LSM6DSL_SPI2_CS_PIN, GPIO_PIN_RESET) : HAL_GPIO_WritePin(LSM6DSL_SPI2_CS_PORT, LSM6DSL_SPI2_CS_PIN, GPIO_PIN_SET);
}

// ***************************************************************************
// Fuction      : Chip_Select_LIS2MDL()
// Description  : 
// 
//
// ***************************************************************************
static void Chip_Select_LIS2MDL(enum SPI2_CHIP_CS cs)
{
   (cs == SPI2_CHIP_SELECT) ? HAL_GPIO_WritePin(LIS2MDL_SPI_CS_PORT, LIS2MDL_SPI_CS_PIN, GPIO_PIN_RESET) : HAL_GPIO_WritePin(LIS2MDL_SPI_CS_PORT, LIS2MDL_SPI_CS_PIN, GPIO_PIN_SET);
}

// ***************************************************************************
// Fuction      : Chip_Select_LPS22HD()
// Description  : 
// 
//
// ***************************************************************************
static void Chip_Select_LPS22HD(enum SPI2_CHIP_CS cs)
{
   (cs == SPI2_CHIP_SELECT) ? HAL_GPIO_WritePin(LPS22HB_SPI_CS_PORT, LPS22HB_SPI_CS_PIN, GPIO_PIN_RESET) : HAL_GPIO_WritePin(LPS22HB_SPI_CS_PORT, LPS22HB_SPI_CS_PIN, GPIO_PIN_SET);
}

// ***************************************************************************
// Fuction      : spi_read()
// Description  : 
// 
//
// ***************************************************************************
static void spi_read(uint8_t *val)
{
   __disable_irq();
   __HAL_SPI_ENABLE(&g_spi2_sensor_Handle);
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __asm("dsb\n");
   __HAL_SPI_DISABLE(&g_spi2_sensor_Handle);
   __enable_irq();

   while (((&g_spi2_sensor_Handle)->Instance->SR & SPI_FLAG_RXNE) != SPI_FLAG_RXNE)
   {
      ;
   }

   // read the received data
   *val                                                  = *(__IO uint8_t *) &((&g_spi2_sensor_Handle)->Instance->DR);

   while (((&g_spi2_sensor_Handle)->Instance->SR & SPI_FLAG_BSY) == SPI_FLAG_BSY)
   {
      ;
   }
}

// ***************************************************************************
// Fuction      : spi_read_nbytes()
// Description  : 
// 
//
// ***************************************************************************
static void spi_read_nbytes(uint8_t *val, uint16_t nbytes)
{
   // Interrupts should be disabled during this operation
   __disable_irq();
   __HAL_SPI_ENABLE(&g_spi2_sensor_Handle);

   // Transfer loop
   while (nbytes > 1U)
   {
      // Check the RXNE flag
      if ((&g_spi2_sensor_Handle)->Instance->SR & SPI_FLAG_RXNE)
      {
         // read the received data
         *val                                         = *(__IO uint8_t *) &((&g_spi2_sensor_Handle)->Instance->DR);
         val                                          += sizeof(uint8_t);
         nbytes--;
      }
   }

   /* In master RX mode the clock is automaticaly generated on the SPI enable.
   So to guarantee the clock generation for only one data, the clock must be
   disabled after the first bit and before the latest bit of the last Byte received */
   /* __DSB instruction are inserted to garantee that clock is Disabled in the right timeframe */

   __DSB();
   __DSB();
   __HAL_SPI_DISABLE(&g_spi2_sensor_Handle);

   __enable_irq();
  
   while (((&g_spi2_sensor_Handle)->Instance->SR & SPI_FLAG_RXNE) != SPI_FLAG_RXNE)
   {
      ;
   }

   // read the received data
   *val                                                  = *(__IO uint8_t *) &((&g_spi2_sensor_Handle)->Instance->DR);
   while (((&g_spi2_sensor_Handle)->Instance->SR & SPI_FLAG_BSY) == SPI_FLAG_BSY)
   {
      ;
   }
}

// ***************************************************************************
// Fuction      : spi_write()
// Description  : 
// 
//
// ***************************************************************************
static void spi_write(uint8_t val)
{
   // check TXE flag
   while (((&g_spi2_sensor_Handle)->Instance->SR & SPI_FLAG_TXE) != SPI_FLAG_TXE)
   {
      ;
   }

   // Write the data
   *((__IO uint8_t*) &((&g_spi2_sensor_Handle)->Instance->DR)) = val;

   // Wait BSY flag
   while (((&g_spi2_sensor_Handle)->Instance->SR & SPI_SR_TXE) != SPI_SR_TXE)
   {
      ;
   }
   while (((&g_spi2_sensor_Handle)->Instance->SR & SPI_FLAG_BSY) == SPI_FLAG_BSY)
   {
      ;
   }
}


// ***************************************************************************
// Fuction      : Sensor_SPI2_Init()
// Description  : 
// 
//
// ***************************************************************************
void Sensor_SPI2_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct;

   UNUSED(spi_write);
   UNUSED(spi_read_nbytes);
   UNUSED(spi_read);
   UNUSED(Chip_Select_LPS22HD);
   UNUSED(Chip_Select_LIS2MDL);
   UNUSED(Chip_Select_LSM6DSL);

   __GPIOA_CLK_ENABLE();
   __GPIOB_CLK_ENABLE();
   __GPIOC_CLK_ENABLE();

   HAL_GPIO_WritePin(LSM6DSL_SPI2_CS_PORT, LSM6DSL_SPI2_CS_PIN, GPIO_PIN_SET);
   HAL_GPIO_WritePin(LPS22HB_SPI_CS_PORT, LPS22HB_SPI_CS_PIN, GPIO_PIN_SET);
   HAL_GPIO_WritePin(LIS2MDL_SPI_CS_PORT, LIS2MDL_SPI_CS_PIN, GPIO_PIN_SET);

   // CS : LSM6DSL
   GPIO_InitStruct.Pin                                   = LSM6DSL_SPI2_CS_PIN;
   GPIO_InitStruct.Mode                                  = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull                                  = GPIO_NOPULL;
   GPIO_InitStruct.Speed                                 = GPIO_SPEED_HIGH;
   GPIO_InitStruct.Alternate                             = 0;
   HAL_GPIO_Init(LSM6DSL_SPI2_CS_PORT, &GPIO_InitStruct);
   HAL_GPIO_WritePin(LSM6DSL_SPI2_CS_PORT, LSM6DSL_SPI2_CS_PIN, GPIO_PIN_SET);

   // CS : LIS2MDL
   GPIO_InitStruct.Pin                                   = LPS22HB_SPI_CS_PIN;
   GPIO_InitStruct.Mode                                  = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull                                  = GPIO_NOPULL;
   GPIO_InitStruct.Speed                                 = GPIO_SPEED_HIGH;
   GPIO_InitStruct.Alternate                             = 0;
   HAL_GPIO_Init(LPS22HB_SPI_CS_PORT, &GPIO_InitStruct);
   HAL_GPIO_WritePin(LPS22HB_SPI_CS_PORT, LPS22HB_SPI_CS_PIN, GPIO_PIN_SET);

   // CS : LPS22HD
   GPIO_InitStruct.Pin                                   = LIS2MDL_SPI_CS_PIN;
   GPIO_InitStruct.Mode                                  = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull                                  = GPIO_NOPULL;
   GPIO_InitStruct.Speed                                 = GPIO_SPEED_HIGH;
   GPIO_InitStruct.Alternate                             = 0;
   HAL_GPIO_Init(LIS2MDL_SPI_CS_PORT, &GPIO_InitStruct);
   HAL_GPIO_WritePin(LIS2MDL_SPI_CS_PORT, LIS2MDL_SPI_CS_PIN, GPIO_PIN_SET);

   g_spi2_sensor_Handle.Instance                         = SPI2;
   g_spi2_sensor_Handle.Init.Mode                        = SPI_MODE_MASTER;
   g_spi2_sensor_Handle.Init.Direction                   = SPI_DIRECTION_1LINE;
   g_spi2_sensor_Handle.Init.DataSize                    = SPI_DATASIZE_8BIT;
   g_spi2_sensor_Handle.Init.CLKPolarity                 = SPI_POLARITY_HIGH;
   g_spi2_sensor_Handle.Init.CLKPhase                    = SPI_PHASE_2EDGE;
   g_spi2_sensor_Handle.Init.NSS                         = SPI_NSS_SOFT;
   g_spi2_sensor_Handle.Init.FirstBit                    = SPI_FIRSTBIT_MSB;
   g_spi2_sensor_Handle.Init.TIMode                      = SPI_TIMODE_DISABLED;
   g_spi2_sensor_Handle.Init.CRCPolynomial               = 7;
   g_spi2_sensor_Handle.Init.BaudRatePrescaler           = SPI_BAUDRATEPRESCALER_16;
   g_spi2_sensor_Handle.Init.CRCCalculation              = SPI_CRCCALCULATION_DISABLED;

   if (HAL_SPI_Init(&g_spi2_sensor_Handle) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   SPI_1LINE_TX(&g_spi2_sensor_Handle);
   __HAL_SPI_ENABLE(&g_spi2_sensor_Handle);

#if defined(LSM6DSL)
   Sensor_lsm6dsl_Init();
#endif

#if defined(LIS2MDL)
   Sensor_lis2mdl_Init();
#endif

#if defined(LPS22HD)
   Sensor_lps22hd_Init();
#endif


}


// ***************************************************************************
// Fuction      : Sensor_SPI2_Read()
// Description  : 
// 
//
// ***************************************************************************
uint8_t Sensor_SPI2_Read(enum SPI2_SENSOR sensor, uint8_t addr, uint8_t* pBuffer, uint16_t nbytes)
{
   switch (sensor)
   {
      case SPI2_LSM6DSL :
         Chip_Select_LSM6DSL(SPI2_CHIP_SELECT);
         addr                                            |= LSM6DSL_SPI_READ_ADDR;
         break;

      case SPI2_LIS2MDL :
         Chip_Select_LIS2MDL(SPI2_CHIP_SELECT);
         break;

      case SPI2_LPS22HD :
         Chip_Select_LPS22HD(SPI2_CHIP_SELECT);
         break;

      default :
         return -1;
         break;
   }

   spi_write(addr);
   __HAL_SPI_DISABLE(&g_spi2_sensor_Handle);
   SPI_1LINE_RX(&g_spi2_sensor_Handle);

   if (nbytes > 1U)
   {
      spi_read_nbytes(pBuffer, nbytes);
   }
   else
   {
      spi_read(pBuffer);
   }

   switch (sensor)
   {
      case SPI2_LSM6DSL :
         Chip_Select_LSM6DSL(SPI2_CHIP_UNSELECT);
         break;

      case SPI2_LIS2MDL :
         Chip_Select_LIS2MDL(SPI2_CHIP_UNSELECT);
         break;

      case SPI2_LPS22HD :
         Chip_Select_LPS22HD(SPI2_CHIP_UNSELECT);
         break;
   }

   // Change the data line to output and enable the SPI
   SPI_1LINE_TX(&g_spi2_sensor_Handle);
   __HAL_SPI_ENABLE(&g_spi2_sensor_Handle);

   return 0;
}


// ***************************************************************************
// Fuction      : Sensor_SPI2_Write()
// Description  : 
// 
//
// ***************************************************************************
uint8_t Sensor_SPI2_Write(enum SPI2_SENSOR sensor, uint8_t addr, uint8_t* pBuffer, uint16_t nbytes)
{
   int i;
   switch (sensor)
   {
      case SPI2_LSM6DSL :
         Chip_Select_LSM6DSL(SPI2_CHIP_SELECT);
         break;

      case SPI2_LIS2MDL :
         Chip_Select_LIS2MDL(SPI2_CHIP_SELECT);
         break;

      case SPI2_LPS22HD :
         Chip_Select_LPS22HD(SPI2_CHIP_SELECT);
         break;

      default :
         return -1;
         break;
   }

   spi_write(addr);

   for (i = 0; i < nbytes; i++)
   {
      spi_write(pBuffer[i]);
   }

   switch (sensor)
   {
      case SPI2_LSM6DSL :
         Chip_Select_LSM6DSL(SPI2_CHIP_UNSELECT);
         break;

      case SPI2_LIS2MDL :
         Chip_Select_LIS2MDL(SPI2_CHIP_UNSELECT);
         break;

      case SPI2_LPS22HD :
         Chip_Select_LPS22HD(SPI2_CHIP_UNSELECT);
         break;
   }
   return 0;
}
#endif

