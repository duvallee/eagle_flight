/*
 * File: steval_fcu001v1_driver.c
 *
 * Written by duvallee in 2018
 *
 */
#include "main.h"
#include "steval_fcu001v1_driver.h"
#include "sensor_spi2.h"
#include "lsm6dsl.h"

// ---------------------------------------------------------------------------
#define LSM6DSL_SPI2_CS_PORT	                           GPIOA
#define LSM6DSL_SPI2_CS_PIN     	                        GPIO_PIN_8

#define LPS22HB_SPI_CS_PORT	                           GPIOC
#define LPS22HB_SPI_CS_PIN     	                        GPIO_PIN_13

#define LIS2MDL_SPI_CS_PORT	                           GPIOB
#define LIS2MDL_SPI_CS_PIN     	                        GPIO_PIN_12

// ***************************************************************************
// Fuction      : Sensor_lsm6dsl_Init()
// Description  : 
// 
//
// ***************************************************************************
void Board_Driver_Init()
{
   void* pSpi;
   SENSOR_LSM6DSL_STRUCT lsm6dsl;

   // -----------------------------------------------------------------------------------------------------------------------
   pSpi                                                  = Sensor_SPI2_Init(LSM6DSL_SPI2_CS_PORT, LSM6DSL_SPI2_CS_PIN, SPI2);
   lsm6dsl.spi                                           = pSpi;
   lsm6dsl.chip_select_port                              = (void*) LSM6DSL_SPI2_CS_PORT;
   lsm6dsl.chip_select_pin                               = LSM6DSL_SPI2_CS_PIN;
   lsm6dsl.read_fn                                       = Sensor_SPI2_Read;
   lsm6dsl.write_fn                                      = Sensor_SPI2_Write;
   Sensor_lsm6dsl_Init(&lsm6dsl);

   // -----------------------------------------------------------------------------------------------------------------------
   pSpi                                                  = Sensor_SPI2_Init(LPS22HB_SPI_CS_PORT, LPS22HB_SPI_CS_PIN, SPI2);

   // -----------------------------------------------------------------------------------------------------------------------
   pSpi                                                  = Sensor_SPI2_Init(LIS2MDL_SPI_CS_PORT, LIS2MDL_SPI_CS_PIN, SPI2);


// uint8_t Sensor_SPI2_Read(void* spi, void* port, uint32_t pin, uint8_t addr, uint8_t* pBuffer, uint16_t nbytes);
// uint8_t Sensor_SPI2_Write(void* spi, void* port, uint32_t pin, uint8_t addr, uint8_t* pBuffer, uint16_t nbytes);


#if 0
   led_init();
   set_led_1_mode(LED_BLINK_MODE);
   Battery_Gauge_Init();
   Motor_Init();

   Motor_Ext_Init();

#if defined(TIM2_REMOTE_CONTROL)
   RemoteController_Init();
#endif   // TIM2_REMOTE_CONTROL

   BLUE_NRG_SPI1_Init();
   Sensor_SPI2_Init();

#endif
}

