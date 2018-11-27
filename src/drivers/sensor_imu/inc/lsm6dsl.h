/*
 * File: lsm6dsl.h
 *
 * Written by duvallee in 2018
 *
 */

#ifndef __LSM6DSL_H__
#define __LSM6DSL_H__
#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------------------
typedef uint8_t (*LSM6DSL_READ_FN)(void* spi, void* port, uint32_t pin, uint8_t* pBuffer, uint16_t nbytes);
typedef uint8_t (*LSM6DSL_WRITE_FN)(void* spi, void* port, uint32_t pin, uint8_t* pBuffer, uint16_t nbytes);

typedef struct tag_SENSOR_LSM6DSL_STRUCT
{
   void* spi;
   void* chip_select_port;
   uint32_t chip_select_pin;

   LSM6DSL_READ_FN read_fn;
   LSM6DSL_WRITE_FN write_fn;
} SENSOR_LSM6DSL_STRUCT;


// --------------------------------------------------------------------------------
void Sensor_lsm6dsl_Init(SENSOR_LSM6DSL_STRUCT* pLSM6DSL);

#ifdef __cplusplus
}
#endif

#endif      // __LSM6DSL_H__

