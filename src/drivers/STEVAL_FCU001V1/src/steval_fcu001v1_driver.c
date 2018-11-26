/*
 * File: steval_fcu001v1_driver.c
 *
 * Written by duvallee in 2018
 *
 */
#include "main.h"
#include "steval_fcu001v1_driver.h"

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

