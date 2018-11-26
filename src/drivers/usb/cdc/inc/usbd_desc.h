/*
 * File: usbd_desc.h
 *
 * Written by duvallee.lee in 2018
 *
 */

#ifndef __USBD_DESC_H__
#define __USBD_DESC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "usbd_def.h"

#define DEVICE_ID1                                       (0x5C001000)
#define DEVICE_ID2                                       (0x5C001004)
#define DEVICE_ID3                                       (0x5C001008)

#define USB_SIZ_STRING_SERIAL                            0x1A
extern USBD_DescriptorsTypeDef VCP_Desc;

#ifdef __cplusplus
}
#endif

#endif   // __USBD_DESC_H__

