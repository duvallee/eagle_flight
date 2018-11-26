/*
 * File: battery_gauge.h
 *
 * Written by duvallee in 2018
 *
 */

#ifndef __BATTERY_GAUGE_H__
#define __BATTERY_GAUGE_H__
#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------------------
#define VBAT_SENSE_GPIO_B_1                              GPIO_PIN_1
#define VBAT_SENSE_PORT                                  GPIOB

// --------------------------------------------------------------------------------
void Battery_Gauge_Init(void);


#ifdef __cplusplus
}
#endif

#endif      // __BATTERY_GAUGE_H__

