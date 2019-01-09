/*
 *  File: vl53l0x_platform.c
 *
 * Written by ST
 *
 */
#include <string.h>
#include "main.h"

#include "vl53l0x_platform.h"
#include "vl53l0x_api.h"

#define I2C_TIME_OUT_BASE                                10
#define I2C_TIME_OUT_BYTE                                1


#ifndef HAL_I2C_MODULE_ENABLED
#error "HAL I2C module must be enable "
#endif

// when not customized by application define dummy one
#ifndef VL53L0X_GetI2cBus
// This macro can be overloaded by user to enforce i2c sharing in RTOS context
#if defined(RTOS_FREERTOS)
void VL53L0X_GetI2cBus(VL53L0X_DEV Dev)
{
//   xSemaphoreTake(*(Dev->i2c_semaphore), portMAX_DELAY);
}
#else
#define VL53L0X_GetI2cBus(...)                           (void) 0
#endif   // RTOS_FREERTOS
#endif   // VL53L0X_GetI2cBus

#ifndef VL53L0X_PutI2cBus
#if defined(RTOS_FREERTOS)
void VL53L0X_PutI2cBus(VL53L0X_DEV Dev)
{
//   xSemaphoreGive(*(Dev->i2c_semaphore));
}
#else
// This macro can be overloaded by user to enforce i2c sharing in RTOS context
#define VL53L0X_PutI2cBus(...)                           (void) 0
#endif   // RTOS_FREERTOS
#endif   // VL53L0X_PutI2cBus

#ifndef VL53L0X_OsDelay
#if defined(RTOS_FREERTOS)
void VL53L0X_OsDelay(uint32_t milli_second)
{
//   osDelay(milli_second);
//   HAL_Delay(milli_second);
}
#else
#define VL53L0X_OsDelay(x)                               HAL_Delay(x)
#endif
#endif


// ===============================================================================
static uint8_t _I2CBuffer[64];

/* --------------------------------------------------------------------------
 * Name : _I2CWrite()
 *
 *
 * -------------------------------------------------------------------------- */
int _I2CWrite(VL53L0X_DEV Dev, uint8_t *pdata, uint32_t count)
{
   int status;
   int i2c_time_out                                      = I2C_TIME_OUT_BASE + count * I2C_TIME_OUT_BYTE;

   status                                                = HAL_I2C_Master_Transmit(Dev->I2cHandle, Dev->I2cDevAddr, pdata, count, i2c_time_out);
   if (status)
   {
      debug_output_error("I2C error 0x%x %d len \r\n", Dev->I2cDevAddr, (int) count);
   }
   return status;
}

/* --------------------------------------------------------------------------
 * Name : _I2CRead()
 *
 *
 * -------------------------------------------------------------------------- */
int _I2CRead(VL53L0X_DEV Dev, uint8_t *pdata, uint32_t count)
{
   int status;
   int i2c_time_out                                      = I2C_TIME_OUT_BASE + count * I2C_TIME_OUT_BYTE;

   status                                                = HAL_I2C_Master_Receive(Dev->I2cHandle, Dev->I2cDevAddr | 1, pdata, count, i2c_time_out);
   if (status)
   {
      debug_output_error("I2C error 0x%x %d len \r\n", Dev->I2cDevAddr | 1, (int) count);
   }
   return status;
}

/* --------------------------------------------------------------------------
 * Name : VL53L0X_WriteMulti()
 *  the ranging_sensor_comms.dll will take care of the page selection
 *
 * -------------------------------------------------------------------------- */
VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count)
{
   int status_int;
   VL53L0X_Error Status                                  = VL53L0X_ERROR_NONE;
   if (count > sizeof(_I2CBuffer) - 1)
   {
      return VL53L0X_ERROR_INVALID_PARAMS;
   }
   _I2CBuffer[0]                                         = index;
   memcpy(&_I2CBuffer[1], pdata, count);
   VL53L0X_GetI2cBus(Dev);
   status_int                                            = _I2CWrite(Dev, _I2CBuffer, count + 1);
   if (status_int != 0)
   {
      Status                                             = VL53L0X_ERROR_CONTROL_INTERFACE;
   }
   VL53L0X_PutI2cBus(Dev);
   return Status;
}

/* --------------------------------------------------------------------------
 * Name : VL53L0X_ReadMulti()
 *  the ranging_sensor_comms.dll will take care of the page selection
 *
 * -------------------------------------------------------------------------- */
VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count)
{
   VL53L0X_Error Status                                  = VL53L0X_ERROR_NONE;
   int32_t status_int;
   VL53L0X_GetI2cBus(Dev);
   status_int                                            = _I2CWrite(Dev, &index, 1);
   if (status_int != 0)
   {
      Status                                             = VL53L0X_ERROR_CONTROL_INTERFACE;
      VL53L0X_PutI2cBus(Dev);
      return Status;
   }
   status_int                                            = _I2CRead(Dev, pdata, count);
   if (status_int != 0)
   {
      Status                                             = VL53L0X_ERROR_CONTROL_INTERFACE;
      VL53L0X_PutI2cBus(Dev);
      return Status;
   }
   VL53L0X_PutI2cBus(Dev);
   return Status;
}

/* --------------------------------------------------------------------------
 * Name : VL53L0X_WrByte()
 *
 *
 * -------------------------------------------------------------------------- */
VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV Dev, uint8_t index, uint8_t data)
{
   VL53L0X_Error Status                                  = VL53L0X_ERROR_NONE;
   int32_t status_int;

   _I2CBuffer[0]                                         = index;
   _I2CBuffer[1]                                         = data;

   VL53L0X_GetI2cBus(Dev);
   status_int                                            = _I2CWrite(Dev, _I2CBuffer, 2);
   if (status_int != 0)
   {
      Status                                             = VL53L0X_ERROR_CONTROL_INTERFACE;
   }
   VL53L0X_PutI2cBus(Dev);
   return Status;
}

/* --------------------------------------------------------------------------
 * Name : VL53L0X_WrWord()
 *
 *
 * -------------------------------------------------------------------------- */
VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV Dev, uint8_t index, uint16_t data)
{
   VL53L0X_Error Status                                  = VL53L0X_ERROR_NONE;
   int32_t status_int;

   _I2CBuffer[0]                                         = index;
   _I2CBuffer[1]                                         = data >> 8;
   _I2CBuffer[2]                                         = data & 0x00FF;

   VL53L0X_GetI2cBus(Dev);
   status_int                                            = _I2CWrite(Dev, _I2CBuffer, 3);
   if (status_int != 0)
   {
      Status                                             = VL53L0X_ERROR_CONTROL_INTERFACE;
   }
   VL53L0X_PutI2cBus(Dev);
   return Status;
}

/* --------------------------------------------------------------------------
 * Name : VL53L0X_WrDWord()
 *
 *
 * -------------------------------------------------------------------------- */
VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t data)
{
   VL53L0X_Error Status                                  = VL53L0X_ERROR_NONE;
   int32_t status_int;
   _I2CBuffer[0]                                         = index;
   _I2CBuffer[1]                                         = (data >> 24) & 0xFF;
   _I2CBuffer[2]                                         = (data >> 16) & 0xFF;
   _I2CBuffer[3]                                         = (data >> 8)  & 0xFF;
   _I2CBuffer[4]                                         = (data >> 0 ) & 0xFF;
   VL53L0X_GetI2cBus(Dev);
   status_int                                            = _I2CWrite(Dev, _I2CBuffer, 5);
   if (status_int != 0)
   {
      Status                                             = VL53L0X_ERROR_CONTROL_INTERFACE;
   }
   VL53L0X_PutI2cBus(Dev);
   return Status;
}

/* --------------------------------------------------------------------------
 * Name : VL53L0X_UpdateByte()
 *
 *
 * -------------------------------------------------------------------------- */
VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index, uint8_t AndData, uint8_t OrData)
{
   VL53L0X_Error Status                                  = VL53L0X_ERROR_NONE;
   uint8_t data;

   Status                                                = VL53L0X_RdByte(Dev, index, &data);
   if (Status)
   {
      return Status;
   }
   data                                                  = (data & AndData) | OrData;
   Status                                                = VL53L0X_WrByte(Dev, index, data);
   return Status;
}

/* --------------------------------------------------------------------------
 * Name : VL53L0X_RdByte()
 *
 *
 * -------------------------------------------------------------------------- */
VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV Dev, uint8_t index, uint8_t *data)
{
   VL53L0X_Error Status                                  = VL53L0X_ERROR_NONE;
   int32_t status_int;

   VL53L0X_GetI2cBus(Dev);
   status_int                                            = _I2CWrite(Dev, &index, 1);
   if (status_int)
   {
      Status                                             = VL53L0X_ERROR_CONTROL_INTERFACE;
      VL53L0X_PutI2cBus(Dev);
      return Status;
   }
   status_int                                            = _I2CRead(Dev, data, 1);
   if (status_int != 0)
   {
      Status                                             = VL53L0X_ERROR_CONTROL_INTERFACE;
   }
   VL53L0X_PutI2cBus(Dev);
   return Status;
}

/* --------------------------------------------------------------------------
 * Name : VL53L0X_RdWord()
 *
 *
 * -------------------------------------------------------------------------- */
VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV Dev, uint8_t index, uint16_t *data)
{
   VL53L0X_Error Status                                  = VL53L0X_ERROR_NONE;
   int32_t status_int;

   VL53L0X_GetI2cBus(Dev);
   status_int                                            = _I2CWrite(Dev, &index, 1);

   if (status_int)
   {
      Status                                             = VL53L0X_ERROR_CONTROL_INTERFACE;
      VL53L0X_PutI2cBus(Dev);
      return Status;
   }
   status_int                                            = _I2CRead(Dev, _I2CBuffer, 2);
   if (status_int != 0)
   {
      Status                                             = VL53L0X_ERROR_CONTROL_INTERFACE;
      VL53L0X_PutI2cBus(Dev);
      return Status;
   }

   *data                                                 = ((uint16_t) _I2CBuffer[0] << 8) + (uint16_t) _I2CBuffer[1];

   VL53L0X_PutI2cBus(Dev);
   return Status;
}

/* --------------------------------------------------------------------------
 * Name : VL53L0X_RdDWord()
 *
 *
 * -------------------------------------------------------------------------- */
VL53L0X_Error VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t *data)
{
   VL53L0X_Error Status                                  = VL53L0X_ERROR_NONE;
   int32_t status_int;

   VL53L0X_GetI2cBus(Dev);
   status_int                                            = _I2CWrite(Dev, &index, 1);
   if (status_int != 0)
   {
      Status                                             = VL53L0X_ERROR_CONTROL_INTERFACE;
      VL53L0X_PutI2cBus(Dev);
      return Status;
   }
   status_int                                            = _I2CRead(Dev, _I2CBuffer, 4);
   if (status_int != 0)
   {
      Status                                             = VL53L0X_ERROR_CONTROL_INTERFACE;
      VL53L0X_PutI2cBus(Dev);
      return Status;
   }

   *data                                                 = ((uint32_t) _I2CBuffer[0] << 24) + ((uint32_t) _I2CBuffer[1] << 16) + 
                                                           ((uint32_t) _I2CBuffer[2] <<  8) + (uint32_t) _I2CBuffer[3];

   VL53L0X_PutI2cBus(Dev);
   return Status;
}

/* --------------------------------------------------------------------------
 * Name : VL53L0X_PollingDelay()
 *
 *
 * -------------------------------------------------------------------------- */
VL53L0X_Error VL53L0X_PollingDelay(VL53L0X_DEV Dev)
{
   VL53L0X_Error status                               = VL53L0X_ERROR_NONE;

   // do nothing
   VL53L0X_OsDelay(2);
   return status;
}

