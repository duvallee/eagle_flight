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
void usb_read(uint8_t* Buf, uint16_t Len);
void usb_write(uint8_t* Buf, uint16_t Len);

#ifdef __cplusplus
}
#endif

#endif      // __USB_DEVICE_H__


