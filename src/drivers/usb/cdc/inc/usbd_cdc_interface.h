/*
 * File: usbd_cdc_interface.h
 *
 * Written by duvallee.lee in 2018
 *
 */

#ifndef __USBD_CDC_INTERFACE_H__
#define __USBD_CDC_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

int8_t CDC_Itf_Transmitter(uint8_t* pBuffer, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif   // __USBD_CDC_INTERFACE_H__




