/*
 * File: battery_gauge.c
 *
 * Written by duvallee in 2018
 *
*/
#include "main.h"
#include "battery_gauge.h"
#include "led.h"

// ---------------------------------------------------------------------------
#define BATTERY_GAUGE_TIMER                              5000

// ---------------------------------------------------------------------------
// for adc
#define BATTERY_MAX_ADC_VALUE                            4095
#define BATTERY_MAX_ADC_VOLTAGE                          3300

#define BATTERY_UP_REGISTER                              10
#define BATTERY_DOWN_REGISTER                            20

// ---------------------------------------------------------------------------
// for battery
#define BATTERY_FULL_VOLTAGE                             4000
#define BATTERY_NORMAL_VOLTAGE                           3800
#define BATTERY_LOW_VOLTAGE                              3600
#define BATTERY_CUT_OFF_VOLTAGE                          3400

#define BATTERY_NORMAL_BLINK_TIME                        3000
#define BATTERY_LOW_BLINK_TIME                           1000
#define BATTERY_CUT_OFF_BLINK_TIME                       500

// ---------------------------------------------------------------------------
static ADC_HandleTypeDef g_adc_port1;

/* --------------------------------------------------------------------------
 * Name : get_battery_voltage()
 *
 *
 * -------------------------------------------------------------------------- */
uint8_t get_battery_voltage(uint32_t* p_bat_voltage)
{
   uint32_t adc_value;
   if (HAL_ADC_Start(&g_adc_port1) != HAL_OK)
   {
      debug_output_error("HAL_ADC_Start() failed !!! \r\n");
      return ((uint8_t) -1);
   }
   if (HAL_ADC_PollForConversion(&g_adc_port1, 1000000) != HAL_OK)
   {
      debug_output_error("HAL_ADC_PollForConversion() failed !!! \r\n");
      return ((uint8_t) -1);
   }
   adc_value                                             = HAL_ADC_GetValue(&g_adc_port1);
   if (HAL_ADC_Stop(&g_adc_port1) != HAL_OK)
   {
      debug_output_error("HAL_ADC_Stop() failed !!! \r\n");
      return ((uint8_t) -1);
   }

   // MAX Voltage for ADC = (BATTERY_DOWN_REGISTER / (BATTERY_DOWN_REGISTER + BATTERY_UP_REGISTER)) * 4.3V (max voltage of battery)
   //                     = 3.3 V
   //                 ADC = 4096 (12 bits)
   // Current Voltage     = (adc value * BATTERY_MAX_MILI_VOLTAGE) / BATTERY_MAX_ADC_VALUE

   *p_bat_voltage                                        = (((adc_value * BATTERY_MAX_ADC_VOLTAGE) /  BATTERY_MAX_ADC_VALUE) * (BATTERY_UP_REGISTER + BATTERY_DOWN_REGISTER)) / BATTERY_DOWN_REGISTER;
   return 0;
}



// ***************************************************************************
// Fuction      : Battery_Gauge_Timer()
// Description  : 
// 
//
// ***************************************************************************
void Battery_Gauge_Timer(uint32_t system_ms)
{
   uint32_t voltage_value                                = 0;
   get_battery_voltage(&voltage_value);
   debug_output_info("%d mV \r\n", (int) voltage_value);

   if (voltage_value > BATTERY_FULL_VOLTAGE)
   {
      set_led_1_mode(LED_ON_MODE);
   }
   else if (voltage_value > BATTERY_NORMAL_VOLTAGE)
   {
      set_led_1_mode(LED_BLINK_MODE);
      set_led_1_blink_time(BATTERY_NORMAL_BLINK_TIME);
   }
   else if (voltage_value > BATTERY_LOW_VOLTAGE)
   {
      set_led_1_mode(LED_BLINK_MODE);
      set_led_1_blink_time(BATTERY_LOW_BLINK_TIME);
   }
   else
   {
      set_led_1_mode(LED_BLINK_MODE);
      set_led_1_blink_time(BATTERY_CUT_OFF_BLINK_TIME);
   }
}


// ***************************************************************************
// Fuction      : Battery_Gauge_Init()
// Description  : 
// 
//
// ***************************************************************************
void Battery_Gauge_Init(void)
{
   ADC_ChannelConfTypeDef sConfig;

   // Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
   g_adc_port1.Instance                                  = ADC1;
   g_adc_port1.Init.ClockPrescaler                       = ADC_CLOCK_SYNC_PCLK_DIV4;
   g_adc_port1.Init.Resolution                           = ADC_RESOLUTION_12B;
   g_adc_port1.Init.ScanConvMode                         = DISABLE;
   g_adc_port1.Init.ContinuousConvMode                   = DISABLE;
   g_adc_port1.Init.DiscontinuousConvMode                = DISABLE;
   g_adc_port1.Init.ExternalTrigConvEdge                 = ADC_EXTERNALTRIGCONVEDGE_NONE;
   g_adc_port1.Init.ExternalTrigConv                     = ADC_SOFTWARE_START;
   g_adc_port1.Init.DataAlign                            = ADC_DATAALIGN_RIGHT;
   g_adc_port1.Init.NbrOfConversion                      = 1;
   g_adc_port1.Init.DMAContinuousRequests                = DISABLE;
   g_adc_port1.Init.EOCSelection                         = ADC_EOC_SINGLE_CONV;
   if (HAL_ADC_Init(&g_adc_port1) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   // Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
   sConfig.Channel                                       = ADC_CHANNEL_9;
   sConfig.Rank                                          = 1;
   sConfig.SamplingTime                                  = ADC_SAMPLETIME_3CYCLES;
   if (HAL_ADC_ConfigChannel(&g_adc_port1, &sConfig) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   add_timer(BATTERY_GAUGE_TIMER, 0, Battery_Gauge_Timer);
}

