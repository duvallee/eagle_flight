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
// Pin-MAP
// -----------------------------------------------------------------------------
// for Arduino UNO Connector

// -----------------------------------------------------------------------------
// Connector 4 - Pin 1 : USART6_RX (TIM3_CH2, TIM8_CH2, I2S3_MCK, USART6_RX...)
// 5V tolerant I/O
#define ARDUINO_CN4_PIN_01_PORT                          GPIOC
#define ARDUINO_CN4_PIN_01                               GPIO_PIN_7

// Connector 4 - Pin 2 : USART6_RX (TIM3_CH1, TIM8_CH1, I2S2_MCK, USART6_TX...)
// 5V tolerant I/O
#define ARDUINO_CN4_PIN_02_PORT                          GPIOC
#define ARDUINO_CN4_PIN_02                               GPIO_PIN_6

// Connector 4 - Pin 3 : 
// 5V tolerant I/O
#define ARDUINO_CN4_PIN_03_PORT                          GPIOG
#define ARDUINO_CN4_PIN_03                               GPIO_PIN_6

// Connector 4 - Pin 4 : (NJTRST, TIM3_CH1, SPI_MISO, SPI3_MISO, SPI2_NSS/I2S2_WS)
// 5V tolerant I/O
#define ARDUINO_CN4_PIN_04_PORT                          GPIOB
#define ARDUINO_CN4_PIN_04                               GPIO_PIN_4

// Connector 4 - Pin 5 : (USART6_CK)
// 5V tolerant I/O
#define ARDUINO_CN4_PIN_05_PORT                          GPIOG
#define ARDUINO_CN4_PIN_05                               GPIO_PIN_7

// Connector 4 - Pin 6 : (TIM5_CH4, SPI2_NSS/I2S_WS)
// 5V tolerant I/O
#define ARDUINO_CN4_PIN_06_PORT                          GPIOI
#define ARDUINO_CN4_PIN_06                               GPIO_PIN_0

// Connector 4 - Pin 7 : (I2C2_SMBA, SPI5_SCK,TIM12_CH1 ...)
// 5V tolerant I/O
#define ARDUINO_CN4_PIN_07_PORT                          GPIOH
#define ARDUINO_CN4_PIN_07                               GPIO_PIN_6

// Connector 4 - Pin 8 : (TIM8_ETR, SPI2_MOSI/I2S2_SD)
// 5V tolerant I/O
#define ARDUINO_CN4_PIN_08_PORT                          GPIOI
#define ARDUINO_CN4_PIN_08                               GPIO_PIN_3

// Connector 7 - Pin 1 : (TIM8_CH4, SPI2_MISO)
// 5V tolerant I/O
#define ARDUINO_CN7_PIN_01_PORT                          GPIOI
#define ARDUINO_CN7_PIN_01                               GPIO_PIN_2

// Connector 7 - Pin 2 : (JTDI, TIM2_CH1/TIM2_ETR, MDMI_CEC, SPI1_NSS/I2S1_WS, SPI3_NSS/I2S3_WS, UART4_RTS)
// 5V tolerant I/O
#define ARDUINO_CN7_PIN_02_PORT                          GPIOA
#define ARDUINO_CN7_PIN_02                               GPIO_PIN_15

// Connector 7 - Pin 3 : TIM1_CH1 (MCO1, TIM_CH1, TIM8_BKIN2, USART1_CK, OTG_FS_SOF...)
// 5V tolerant I/O
#define ARDUINO_CN7_PIN_03_PORT                          GPIOA
#define ARDUINO_CN7_PIN_03                               GPIO_PIN_8

// Connector 7 - Pin 4 : (RTC_REFIN, TIM1_CH3N, TIM8_CH_3N, SPI2_MOSI/I2S2_SD, TIM12_CH2 ...)
// 5V tolerant I/O
#define ARDUINO_CN7_PIN_04_PORT                          GPIOB
#define ARDUINO_CN7_PIN_04                               GPIO_PIN_15

// Connector 7 - Pin 5 : (TIM1_CH2N, TIM8_CH2N, SPI2_MISO, USART3_CTS, TIM12_CH1 ...)
// 5V tolerant I/O
#define ARDUINO_CN7_PIN_05_PORT                          GPIOB
#define ARDUINO_CN7_PIN_05                               GPIO_PIN_14

// Connector 7 - Pin 6 : LD1 (LED.GREEN) (TIM8_BKIN2, SPI2_SCK/I2S2_CK ...)
// 5V tolerant I/O
#define ARDUINO_CN7_PIN_06_PORT                          GPIOI
#define ARDUINO_CN7_PIN_06                               GPIO_PIN_1

// AVDD & GND : Pin 7 & Pin 8

// Connector 7 - Pin 9 : SDA (TIM4_CH4, TIM11_CH1, I2C1_SDA, SPI2_NSS/I2S2_WS, CAN1_TX ...)
// 5V tolerant I/O
#define ARDUINO_CN7_PIN_09_PORT                          GPIOB
#define ARDUINO_CN7_PIN_09                               GPIO_PIN_9

// Connector 7 - Pin 10 : SCL (TIM2_CH3, I2C1_SCL, SPI2_SCK/I2S2_CK, USART3_TX)
// 5V tolerant I/O
#define ARDUINO_CN7_PIN_10_PORT                          GPIOB
#define ARDUINO_CN7_PIN_10                               GPIO_PIN_8

// -----------------------------------------------------------------------------
// Connector 5 - Pin 1 : WakeUP (TIM2_CH1/TIM2_ETR, TIM5_CH1, TIM8_ETR, USART2_CTS, UART4_TX ...)
// 5V tolerant I/O
#define ARDUINO_CN5_PIN_01_PORT                          GPIOA
#define ARDUINO_CN5_PIN_01                               GPIO_PIN_0

// Connector 5 - Pin 2 : ( ...)
// 5V tolerant I/O
#define ARDUINO_CN5_PIN_02_PORT                          GPIOF
#define ARDUINO_CN5_PIN_02                               GPIO_PIN_10

// Connector 5 - Pin 3 : (SPI5_MOSI, SAI1_FS_B, UART7_CTS, TIM14_CH1, QUADSPI_BK1_IO1 ...)
// 5V tolerant I/O
#define ARDUINO_CN5_PIN_03_PORT                          GPIOF
#define ARDUINO_CN5_PIN_03                               GPIO_PIN_9

// Connector 5 - Pin 4 : (SPI5_MISO, SAI1_SCK_B, UART7_RTS, TIM13_CH1, QUADSPI_BK1_IO0 ...)
// 5V tolerant I/O
#define ARDUINO_CN5_PIN_04_PORT                          GPIOF
#define ARDUINO_CN5_PIN_04                               GPIO_PIN_8

// Connector 5 - Pin 5 : (TIM11_CH1, SPI5_SCK, SAI1_MCLK_B, UART7_TX ...)
// 5V tolerant I/O
#define ARDUINO_CN5_PIN_05_PORT                          GPIOF
#define ARDUINO_CN5_PIN_05                               GPIO_PIN_7

// Connector 5 - Pin 6 : (TIM10_CH1, SPI5_NSS, SAI_SD_B, UART7_RX ...)
// 5V tolerant I/O
#define ARDUINO_CN5_PIN_06_PORT                          GPIOF
#define ARDUINO_CN5_PIN_06                               GPIO_PIN_6

// -----------------------------------------------------------------------------
#if defined(P_NUCLEO_53L0A1)
#define P_NUCLEO_53L0A1_SCL_PIN                          GPIO_PIN_8
#define P_NUCLEO_53L0A1_SCL_PORT                         GPIOB

#define P_NUCLEO_53L0A1_SDA_PIN                          GPIO_PIN_9
#define P_NUCLEO_53L0A1_SDA_PORT                         GPIOB

#define P_NUCLEO_53L0A1_GPIO1_L_PIN                      GPIO_PIN_7
#define P_NUCLEO_53L0A1_GPIO1_L_PORT                     GPIOC

#define P_NUCLEO_53L0A1_GPIO1_R_PIN                      GPIO_PIN_10
#define P_NUCLEO_53L0A1_GPIO1_R_PORT                     GPIOF

#define P_NUCLEO_53L0A1_GPIO1_PIN                        GPIO_PIN_9
#define P_NUCLEO_53L0A1_GPIO1_PORT                       GPIOF

#define P_NUCLEO_53L0A1_GPIO_EXT_I2C_ADDR                0x86

#endif

// -----------------------------------------------------------------------------
// LD1 ( shared with Connector 7 - Pin 6)
#define LED_1_GREEN_PORT                                 GPIOI
#define LED_1_GREEN                                      GPIO_PIN_1


#if defined(UART_DEBUG_OUTPUT)
#define UART_DEBUG_BAUDRATE                              115200

#define UART_DEBUG_RX_PIN                                GPIO_PIN_7
#define UART_DEBUG_RX_GPIO_PORT                          GPIOC
#define UART_DEBUG_TX_PIN                                GPIO_PIN_6
#define UART_DEBUG_TX_GPIO_PORT                          GPIOC
#endif

// LCD
#define DISPLAY_CONTROL_PIN                              GPIO_PIN_12
#define DISPLAY_CONTROL_PORT                             GPIOI

// Backlight
#define BACKLIGHT_CONTROL_PIN                            GPIO_PIN_3
#define BACKLIGHT_CONTROL_PORT                           GPIOK

// -----------------------------------------------------------------------------
// LD1 ( shared with Connector 7 - Pin 6)
#define LED_1_GREEN_PORT                                 GPIOI
#define LED_1_GREEN                                      GPIO_PIN_1

// -----------------------------------------------------------------------------
// ULPI
#define USB_HS_ULPI_D0_PIN                               GPIO_PIN_3
#define USB_HS_ULPI_D0_PORT                              GPIOA

#define USB_HS_ULPI_D1_PIN                               GPIO_PIN_0
#define USB_HS_ULPI_D1_PORT                              GPIOB

#define USB_HS_ULPI_D2_PIN                               GPIO_PIN_1
#define USB_HS_ULPI_D2_PORT                              GPIOB

#define USB_HS_ULPI_D3_PIN                               GPIO_PIN_10
#define USB_HS_ULPI_D3_PORT                              GPIOB

#define USB_HS_ULPI_D4_PIN                               GPIO_PIN_11
#define USB_HS_ULPI_D4_PORT                              GPIOB

#define USB_HS_ULPI_D5_PIN                               GPIO_PIN_12
#define USB_HS_ULPI_D5_PORT                              GPIOB

#define USB_HS_ULPI_D6_PIN                               GPIO_PIN_13
#define USB_HS_ULPI_D6_PORT                              GPIOB

#define USB_HS_ULPI_D7_PIN                               GPIO_PIN_5
#define USB_HS_ULPI_D7_PORT                              GPIOB

#define USB_HS_ULPI_NXT_PIN                              GPIO_PIN_4
#define USB_HS_ULPI_NXT_PORT                             GPIOH

#define USB_HS_ULPI_DIR_PIN                              GPIO_PIN_2
#define USB_HS_ULPI_DIR_PORT                             GPIOC

#define USB_HS_ULPI_STP_PIN                              GPIO_PIN_0
#define USB_HS_ULPI_STP_PORT                             GPIOC

#define USB_HS_ULPI_CK_PIN                               GPIO_PIN_5
#define USB_HS_ULPI_CK_PORT                              GPIOA

#define USB_HS_ULPI_OTG_OVERCURRENT_PIN                  GPIO_PIN_3
#define USB_HS_ULPI_OTG_OVERCURRENT_PORT                 GPIOE

// TOUCH
#define TOUCH_FT5536_INT_PIN                             GPIO_PIN_13
#define TOUCH_FT5536_INT_PORT                            GPIOI

#define TOUCH_FT5536_SCL_PIN                             GPIO_PIN_7
#define TOUCH_FT5536_SCL_PORT                            GPIOH

#define TOUCH_FT5536_SDA_PIN                             GPIO_PIN_8
#define TOUCH_FT5536_SDA_PORT                            GPIOH

// --------------------------------------------------------------------------
#if defined(USE_USB_CDC_DEVICE)
#define USBD_CDC_BAUDRATE                                115200
#endif

#if (defined(USE_USB_CDC_DEVICE) || defined(USE_USB_BULK_DEVICE))
#define USB_MAX_RECEIVE_BUFFER_SIZE                      2048
#endif

// -----------------------------------------------------------------------------
// LCD
#define RK043FN48H_WIDTH                                 480                     // LCD Pixel Width
#define RK043FN48H_HEIGHT                                272                     // LCD Pixel Height

#define RK043FN48H_HSYNC                                 41                      // Horizontal Synchronization
#define RK043FN48H_HBP                                   13                      // Horizontal Back Porch
#define RK043FN48H_HFP                                   32                      // Horizontal Front Porch
#define RK043FN48H_VSYNC                                 10                      // Vertical Synchronization
#define RK043FN48H_VBP                                   2                       // Vertical Back Porch
#define RK043FN48H_VFP                                   2                       // Vertical Front Porch

#define LCD_WIDTH                                        480
#define LCD_HEIGHT                                       272
#define LCD_PIXEL_BYTE                                   4

#define LCD_FRAMEBUFFER_SIZE                             (LCD_WIDTH * LCD_HEIGHT * LCD_PIXEL_BYTE)

// --------------------------------------------------------------------------
#define ETHERNET_SRAM_BASE_ADDRESS                       0x20010000

// --------------------------------------------------------------------------
// SDRAM (16 MB) : 0xC000 0000 -- 0xC0FF FFFF
#define STEMWIN_GUI_MEM_BASE                             0xC0100000              // 0xC010 0000 -- 0xC01F FFFF
#define STEMWIN_GUI_MEM_SIZE                             0x100000

#define LCD_FRAMEBUFFER_LAYER_0                          0xC0200000
#define LCD_FRAMEBUFFER_LAYER_1                          0xC0400000

#define DMA2D_FRAMEBUFFER_LAYER_0                        0xC0600000
#define DMA2D_FRAMEBUFFER_LAYER_1                        0xC0800000

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


