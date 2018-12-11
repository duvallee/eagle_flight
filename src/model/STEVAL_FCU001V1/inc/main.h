/*
 *  File: main.h
 *
 * Written by duvallee.lee in 2018
 *
 */

#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------------
#if defined(UART_DEBUG_OUTPUT)
#define UART_DEBUG_PORT                                  1
#define UART_DEBUG_BAUDRATE                              115200

#define UART_DEBUG_RX_PIN                                GPIO_PIN_10
#define UART_DEBUG_RX_GPIO_PORT                          GPIOA
#define UART_DEBUG_TX_PIN                                GPIO_PIN_9
#define UART_DEBUG_TX_GPIO_PORT                          GPIOA
#endif

#if defined(USE_USB_CDC_DEVICE)
#define USBD_CDC_BAUDRATE                                115200
#endif

#if (defined(USE_USB_CDC_DEVICE) || defined(USE_USB_BULK_DEVICE))
#define USB_MAX_RECEIVE_BUFFER_SIZE                      256
#endif

// --------------------------------------------------------------------------
#include "string.h"
#if defined(SUPPORT_DEBUG_OUTPUT)
#include "printf.h"
#if defined(UART_DEBUG_OUTPUT)
#include "uart_debug.h"
#endif
#endif
#include "debug_output.h"
#include "stm32f4xx_hal.h"

#if defined(RTOS_FREERTOS)
#include "cmsis_os.h"
#else
#include "scheduler.h"
#endif

#if defined(STEMWIN)
#include "GUI.h"

#include "LCDConf.h"
#include "GUI_Private.h"

#include "WM.h"
#endif

#include "ring_buffer.h"

// --------------------------------------------------------------------------
#define VERSION_MAIN                                     0
#define VERSION_MINOR                                    1
#define VERSION_SUB                                      0

#define PROGRAM_NAME                                     "STM32F401CC_BOOTLOADER"

// --------------------------------------------------------------------------
#if defined(STM32H743XI)
void ns_10_delay(int n_10_sec);
void us_delay(int usec);
void ms_delay(int msec);
#endif

void SystemClock_Config(void);

// --------------------------------------------------------------------------
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif   // __MAIN_H__



