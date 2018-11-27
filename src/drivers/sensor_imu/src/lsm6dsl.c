/*
 * File:lsm6dsl.c
 *
 * Written by duvallee in 2018
 *
 */
#include "main.h"
#include "lsm6dsl.h"

// --------------------------------------------------------------------------------
static SENSOR_LSM6DSL_STRUCT gSensor_LSM6DSL;


// --------------------------------------------------------------------------------
#define LSM6DSL_WHO_AM_I_REG                             0x0F
#define LSM6DSL_WHO_AM_I                                 0x6A


// --------------------------------------------------------------------------------
#define LSM6DSL_CTRL4_C_REG                              0x13
#define LSM6DSL_CTRL4_C_I2C_DISABLE                      0x04

#define LSM6DSL_SPI_READ_ADDR                            0x80


// ***************************************************************************
// Fuction      : Sensor_SPI2_Read()
// Description  : 
// 
//
// ***************************************************************************
static uint8_t Sensor_SPI2_Read(uint8_t addr, uint8_t* pBuffer, uint16_t nbytes)
{
   addr                                                  |= LSM6DSL_SPI_READ_ADDR;
   gSensor_LSM6DSL.write_fn(gSensor_LSM6DSL.spi, gSensor_LSM6DSL.chip_select_port, gSensor_LSM6DSL.chip_select_pin, &addr, 1);
   gSensor_LSM6DSL.read_fn(gSensor_LSM6DSL.spi, gSensor_LSM6DSL.chip_select_port, gSensor_LSM6DSL.chip_select_pin, pBuffer, 1);

   return 0;
}


// ***************************************************************************
// Fuction      : Sensor_SPI2_Write()
// Description  : 
// 
//
// ***************************************************************************
static uint8_t Sensor_SPI2_Write(uint8_t addr, uint8_t* pBuffer, uint16_t nbytes)
{
   gSensor_LSM6DSL.write_fn(gSensor_LSM6DSL.spi, gSensor_LSM6DSL.chip_select_port, gSensor_LSM6DSL.chip_select_pin, &addr, 1);
   gSensor_LSM6DSL.write_fn(gSensor_LSM6DSL.spi, gSensor_LSM6DSL.chip_select_port, gSensor_LSM6DSL.chip_select_pin, pBuffer, nbytes);

   return 0;
}



// ***************************************************************************
// Fuction      : LSM6DSL_I2C_DISABLE()
// Description  : 
// 
//
// ***************************************************************************
static int LSM6DSL_I2C_DISABLE(void)
{
   uint8_t reg_data;
   if (Sensor_SPI2_Read(LSM6DSL_CTRL4_C_REG, &reg_data, 1) != 0)
   {
      debug_output_error("Sensor_SPI2_Read() failed !!! \r\n");
      return -1;
   }
   reg_data                                              |= LSM6DSL_CTRL4_C_I2C_DISABLE;
   
   if (Sensor_SPI2_Write(LSM6DSL_CTRL4_C_REG, &reg_data, 1) != 0)
   {
      debug_output_error("Sensor_SPI2_Write() failed !!! \r\n");
      return -1;
   }
   return 0;
}

// ***************************************************************************
// Fuction      : Sensor_lsm6dsl_Init()
// Description  : 
// 
//
// ***************************************************************************
void Sensor_lsm6dsl_Init(SENSOR_LSM6DSL_STRUCT* pLSM6DSL)
{
   uint8_t reg_data;
   if (pLSM6DSL == NULL)
   {
      debug_output_error("Wrong Parameter \r\n");
      _Error_Handler(__FILE__, __LINE__);
   }

   gSensor_LSM6DSL                                       = *pLSM6DSL;

   if (Sensor_SPI2_Read(LSM6DSL_WHO_AM_I_REG, &reg_data, 1) == 0)
   {
      debug_output_info("Found LSM6DSL !!! \r\n");
      debug_output_info("Who am i (0x6A) : 0x%02X \r\n", reg_data);
   }
   else
   {
      debug_output_error("Not Found LSM6DSL !!! \r\n");
      _Error_Handler(__FILE__, __LINE__);
   }
   LSM6DSL_I2C_DISABLE();
}

