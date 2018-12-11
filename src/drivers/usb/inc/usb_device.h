/*
 * File: usb_device.h
 *
 * Written by duvallee in 2018
 *
 */

#ifndef __USB_DEVICE_H__
#define __USB_DEVICE_H__
#ifdef __cplusplus
extern "C" {
#endif

int usb_device_init(void);
int usb_write(uint8_t* Buf, uint16_t Len);
int usb_get_data(uint8_t* Buf, uint16_t Len);

#if defined(RTOS_FREERTOS)
osSemaphoreId get_usb_device_semaphore(void);
#endif


#ifdef __cplusplus
}
#endif

#endif      // __USB_DEVICE_H__


