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

// --------------------------------------------------------------------------
#include "string.h"
#if defined(SUPPORT_DEBUG_OUTPUT)
#include "printf.h"
#if defined(UART_DEBUG_OUTPUT)
#include "uart_debug.h"
#endif
#endif
#include "debug_output.h"
#include "stm32f7xx.h"
#include "scheduler.h"

// --------------------------------------------------------------------------
#define VERSION_MAIN                                     0
#define VERSION_MINOR                                    1
#define VERSION_SUB                                      0

#define PROGRAM_NAME                                     "STM32F7-DISCOVERY"

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


