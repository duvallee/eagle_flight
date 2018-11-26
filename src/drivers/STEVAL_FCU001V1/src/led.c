/*
 * File: main.c
 *
 * Written by duvallee.lee in 2018
 *
*/

#include "main.h"
#include "led.h"

// ==========================================================================
#define MILLI_SECOND                                     1000
#define DEFAULT_LED_BLINK_MILLI_SECOND                    500

enum
{
   LED_OFF                                               = 0,
   LED_ON                                                = 1,
};

// ==========================================================================
typedef struct tag_LED_CONTROL
{
   uint8_t led_1_mode;
   uint8_t led_1_status;
   uint16_t led_1_blink_time;
   uint16_t led_1_expire_time;

   uint8_t led_2_mode;
   uint8_t led_2_status;
   uint16_t led_2_blink_time;
   uint16_t led_2_expire_time;
} LED_CONTROL;

// ==========================================================================
static LED_CONTROL gLedControl;


/* --------------------------------------------------------------------------
 * Name : led_1_control()
 *
 *
 * -------------------------------------------------------------------------- */
static void led_1_control(uint8_t on)
{
   HAL_GPIO_WritePin(LED_1_PORT, LED_1_GPIO_B_5, on != LED_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* --------------------------------------------------------------------------
 * Name : led_2_control()
 *
 *
 * -------------------------------------------------------------------------- */
static void led_2_control(uint8_t on)
{
   HAL_GPIO_WritePin(LED_2_PORT, LED_2_GPIO_B_4, on != LED_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* --------------------------------------------------------------------------
 * Name : led_tick_timer_handler()
 *
 *
 * -------------------------------------------------------------------------- */
void led_tick_timer_handler()
{
   switch (gLedControl.led_1_mode)
   {
      case LED_OFF_MODE :
         led_1_control(LED_OFF);
         break;

      case LED_ON_MODE :
         led_1_control(LED_ON);
         break;

      case LED_BLINK_MODE :
         gLedControl.led_1_expire_time++;
         if (gLedControl.led_1_expire_time >= gLedControl.led_1_blink_time)
         {
            gLedControl.led_1_expire_time                = 0;
            if (gLedControl.led_1_status == LED_OFF)
            {
               gLedControl.led_1_status                  = LED_ON;
            }
            else
            {
               gLedControl.led_1_status                  = LED_OFF;
            }
            led_1_control(gLedControl.led_1_status);
         }
         break;
   }

   switch (gLedControl.led_2_mode)
   {
      case LED_OFF_MODE :
         led_2_control(LED_OFF);
         break;

      case LED_ON_MODE :
         led_2_control(LED_ON);
         break;

      case LED_BLINK_MODE :
         gLedControl.led_2_expire_time++;
         if (gLedControl.led_2_expire_time >= gLedControl.led_2_blink_time)
         {
            gLedControl.led_2_expire_time                = 0;
            if (gLedControl.led_2_status == LED_OFF)
            {
               gLedControl.led_2_status                  = LED_ON;
            }
            else
            {
               gLedControl.led_2_status                  = LED_OFF;
            }
            led_2_control(gLedControl.led_2_status);
         }
         break;
   }
}


/* --------------------------------------------------------------------------
 * Name : led_init()
 *
 *
 * -------------------------------------------------------------------------- */
void led_init()
{
   GPIO_InitTypeDef GPIO_InitStruct;

   gLedControl.led_1_mode                                = LED_OFF_MODE;
   gLedControl.led_1_status                              = LED_OFF;
   gLedControl.led_1_blink_time                          = DEFAULT_LED_BLINK_MILLI_SECOND;
   gLedControl.led_1_expire_time                         = 0;

   gLedControl.led_2_mode                                = LED_OFF_MODE;
   gLedControl.led_2_status                              = LED_OFF;
   gLedControl.led_2_blink_time                          = DEFAULT_LED_BLINK_MILLI_SECOND;
   gLedControl.led_2_expire_time                         = 0;

   // --------------------------------------------------------------------------------
   __GPIOB_CLK_ENABLE();

   GPIO_InitStruct.Pin                                   = LED_1_GPIO_B_5;
   GPIO_InitStruct.Mode                                  = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull                                  = GPIO_NOPULL;
   GPIO_InitStruct.Speed                                 = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(LED_1_PORT, &GPIO_InitStruct);
   HAL_GPIO_WritePin(LED_1_PORT, LED_1_GPIO_B_5, GPIO_PIN_RESET);

   // --------------------------------------------------------------------------------
   GPIO_InitStruct.Pin                                   = LED_2_GPIO_B_4;
   GPIO_InitStruct.Mode                                  = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull                                  = GPIO_NOPULL;
   GPIO_InitStruct.Speed                                 = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(LED_2_PORT, &GPIO_InitStruct);
   HAL_GPIO_WritePin(LED_2_PORT, LED_2_GPIO_B_4, GPIO_PIN_RESET);

}

/* --------------------------------------------------------------------------
 * Name : set_led_1_blink_time()
 *
 *
 * -------------------------------------------------------------------------- */
void set_led_1_blink_time(int ms)
{
   gLedControl.led_1_blink_time                          = ms;
}

/* --------------------------------------------------------------------------
 * Name : set_led_2_blink_time()
 *
 *
 * -------------------------------------------------------------------------- */
void set_led_2_blink_time(int ms)
{
   gLedControl.led_2_blink_time                          = ms;
}

/* --------------------------------------------------------------------------
 * Name : set_led_1_mode()
 *
 *
 * -------------------------------------------------------------------------- */
void set_led_1_mode(enum LED_MODE mode)
{
   gLedControl.led_1_mode                                = mode;
}

/* --------------------------------------------------------------------------
 * Name : set_led_2_mode()
 *
 *
 * -------------------------------------------------------------------------- */
void set_led_2_mode(enum LED_MODE mode)
{
   gLedControl.led_2_mode                                = mode;
}


