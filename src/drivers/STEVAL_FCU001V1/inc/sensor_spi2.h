/*
 * File: spi.h
 *
 * Written by duvallee in 2018
 *
 */

#ifndef __SPI_H__
#define __SPI_H__
#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------------------
void* Sensor_SPI2_Init(void);

uint8_t Sensor_SPI2_Read(void* spi, void* port, uint32_t pin, uint8_t addr, uint8_t* pBuffer, uint16_t nbytes);
uint8_t Sensor_SPI2_Write(void* spi, void* port, uint32_t pin, uint8_t addr, uint8_t* pBuffer, uint16_t nbytes);

#ifdef __cplusplus
}
#endif

#endif      // __SPI_H__

