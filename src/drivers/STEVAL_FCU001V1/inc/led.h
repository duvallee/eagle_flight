/*
 *  File: led.h
 *
 *  Written by duvallee.lee in 2018
 *
*/

#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
extern "C" {
#endif 

// ==========================================================================
#define LED_1_GPIO_B_5                                   GPIO_PIN_5
#define LED_1_PORT                                       GPIOB

#define LED_2_GPIO_B_4                                   GPIO_PIN_4
#define LED_2_PORT                                       GPIOB

// ==========================================================================
enum LED_MODE
{
   LED_OFF_MODE                                          = 0,
   LED_ON_MODE                                           = 1,
   LED_BLINK_MODE                                        = 2,
};

// ==========================================================================
void led_init();

void set_led_1_blink_time(int ms);
void set_led_2_blink_time(int ms);
void set_led_1_mode(enum LED_MODE mode);
void set_led_2_mode(enum LED_MODE mode);

#ifdef __cplusplus
}
#endif


#endif   // __LED_H__

