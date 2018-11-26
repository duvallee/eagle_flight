/*
 *  File: usbd_bulk_interface.h
 *
 * Written by duvallee.lee in 2018
 *
*/

#ifndef __USBD_BULK_INTERFACE_H__
#define __USBD_BULK_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

enum BULK_TRANSMIT_STATUS
{
   BULK_TRANSMIT_SUCCESS                                 = 0,
   BULK_TRANSMIT_ERROR_NULL_POINTER,
   BULK_TRANSMIT_ERROR,
   BULK_TRANSMIT_ERROR_TIME_OUT,
   BULK_TRANSMIT_ERROR_UNPLUG_CABLE,
};
enum BULK_TRANSMIT_STATUS BULK_Transmit(uint8_t* Buf, uint16_t Len);

#ifdef __cplusplus
}
#endif

#endif   // __USBD_BULK_INTERFACE_H__




