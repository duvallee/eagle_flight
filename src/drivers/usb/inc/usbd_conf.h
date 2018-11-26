/*
 * File: usbd_conf.h
 *
 * Written by duvallee.lee in 2018
 *
 */

#ifndef __USBD_CONF_H__
#define __USBD_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(USE_USB_BULK_DEVICE)
#endif

#if defined(DUSE_USB_CDC_DEVICE)
#endif

#define USBD_MAX_NUM_INTERFACES                          1
#define USBD_MAX_NUM_CONFIGURATION                       1
#define USBD_MAX_STR_DESC_SIZ                            0x100
#define USBD_SUPPORT_USER_STRING                         0 
#define USBD_SELF_POWERED                                1
#define USBD_DEBUG_LEVEL                                 0
#define USBD_BULKC_INTERVAL                              1000
#define DEVICE_HS                                        0


#define USBD_malloc                                      malloc
#define USBD_free                                        free
#define USBD_memset                                      memset
#define USBD_memcpy                                      memcpy

#define USBD_UsrLog(...)                                 debug_output_info(__VA_ARGS__)
#define USBD_ErrLog(...)                                 debug_output_error(__VA_ARGS__)
#define USBD_DbgLog(...)                                 debug_output_debug(__VA_ARGS__)


#ifdef __cplusplus
}
#endif

#endif   // __USBD_CONF_H__




