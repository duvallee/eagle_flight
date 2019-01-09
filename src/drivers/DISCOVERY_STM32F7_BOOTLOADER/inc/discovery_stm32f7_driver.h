/*
 * File: discovery_stm32f7_driver.h
 *
 * Written by duvallee in 2018
 *
 */

#ifndef __DISCOVERY_STM32F7_DRIVER_H__
#define __DISCOVERY_STM32F7_DRIVER_H__
#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------------------
void Board_Driver_Init();

void Display_Clear(void);
void Display_On(int on);
volatile uint32_t* getFrameBuffer(void);


#ifdef __cplusplus
}
#endif

#endif      // __DISCOVERY_STM32F7_DRIVER_H__

