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
void* Sensor_SPI2_Init(GPIO_TypeDef* port, uint32_t pin, SPI_TypeDef* spi);

uint8_t Sensor_SPI2_Read(void* spi, void* port, uint32_t pin, uint8_t* pBuffer, uint16_t nbytes);
uint8_t Sensor_SPI2_Write(void* spi, void* port, uint32_t pin, uint8_t* pBuffer, uint16_t nbytes);

#ifdef __cplusplus
}
#endif

#endif      // __SENSOR_SPI2_H__

