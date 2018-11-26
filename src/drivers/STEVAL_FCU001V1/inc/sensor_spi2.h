/*
 * File: sensor_spi2.h
 *
 * Written by duvallee in 2018
 *
 */

#ifndef __SENSOR_SPI2_H__
#define __SENSOR_SPI2_H__
#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------------------
// LSM6DSL A+GIMU            LSM6DS33_CS (PA8)
// LIS2MDL Magnetometer      LIS2MDL_CS (PB2)
// LPS22HD Pressure Sensor   LPS22HB_CS (PC13)
enum SPI2_SENSOR
{
   SPI2_LSM6DSL                                          = 1,
   SPI2_LIS2MDL                                          = 2,
   SPI2_LPS22HD                                          = 3,
};

// --------------------------------------------------------------------------------
typedef uint8_t (*SPI2_READ)(uint8_t addr, uint8_t* pBuffer, uint16_t nbytes);
typedef uint8_t (*SPI2_WRITE)(uint8_t addr, uint8_t* pBuffer, uint16_t nbytes);

typedef struct tagSPI2_DeviceDriver_Struct
{
   enum SPI2_SENSOR spi2_sensor;
   SPI2_READ spi2_read;
   SPI2_WRITE spi2_write;
} SPI2_DeviceDriver;


// --------------------------------------------------------------------------------
void Sensor_SPI2_Init(void);
uint8_t Sensor_SPI2_Read(enum SPI2_SENSOR sensor, uint8_t addr, uint8_t* pBuffer, uint16_t nbytes);
uint8_t Sensor_SPI2_Write(enum SPI2_SENSOR sensor, uint8_t addr, uint8_t* pBuffer, uint16_t nbytes);

#ifdef __cplusplus
}
#endif

#endif      // __SENSOR_SPI2_H__

