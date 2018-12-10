/*
 * File: nucleo_h743zi_driver.c
 *
 * Written by duvallee in 2018
 *
 */
#include "main.h"
#include "nucleo_h743zi_driver.h"

// --------------------------------------------------------------------------
static TIM_HandleTypeDef g_htim1_motor;


/* --------------------------------------------------------------------------
 * Name : BSP_TIM1_Init()
 *        
 *
 * -------------------------------------------------------------------------- */
static void BSP_TIM1_Init(void)
{
   TIM_ClockConfigTypeDef sClockSourceConfig;
   TIM_MasterConfigTypeDef sMasterConfig;
   TIM_OC_InitTypeDef sConfigOC;

   g_htim1_motor.Instance                                = TIM1;
#ifdef MOTOR_DC
   g_htim1_motor.Init.Prescaler                          = 84;                                  /* DC motor configuration - Freq 494Hz*/
   g_htim1_motor.Init.CounterMode                        = TIM_COUNTERMODE_UP;
   g_htim1_motor.Init.Period                             = 1999;
#endif

#ifdef MOTOR_ESC
   g_htim1_motor.Init.Prescaler                          = 100;                                 /* ESC motor configuration - Freq 400Hz*/
   g_htim1_motor.Init.CounterMode                        = TIM_COUNTERMODE_UP;
   g_htim1_motor.Init.Period                             = 2075;
#endif
                     
   g_htim1_motor.Init.ClockDivision                      = TIM_CLOCKDIVISION_DIV1;
   if (HAL_TIM_Base_Init(&g_htim1_motor) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   sClockSourceConfig.ClockSource                        = TIM_CLOCKSOURCE_INTERNAL;
   if (HAL_TIM_ConfigClockSource(&g_htim1_motor, &sClockSourceConfig) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   if (HAL_TIM_PWM_Init(&g_htim1_motor) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   sMasterConfig.MasterOutputTrigger                     = TIM_TRGO_RESET;
   sMasterConfig.MasterSlaveMode                         = TIM_MASTERSLAVEMODE_DISABLE;
   HAL_TIMEx_MasterConfigSynchronization(&g_htim1_motor, &sMasterConfig);

   sConfigOC.OCMode                                      = TIM_OCMODE_PWM1;
   sConfigOC.Pulse                                       = 0;
   sConfigOC.OCPolarity                                  = TIM_OCPOLARITY_HIGH;
   sConfigOC.OCFastMode                                  = TIM_OCFAST_DISABLE;
   HAL_TIM_PWM_ConfigChannel(&g_htim1_motor, &sConfigOC, TIM_CHANNEL_1);
   HAL_TIM_PWM_ConfigChannel(&g_htim1_motor, &sConfigOC, TIM_CHANNEL_2);
}

/* --------------------------------------------------------------------------
 * Name : BSP_TIM1_Start()
 *        
 *
 * -------------------------------------------------------------------------- */
static void BSP_TIM1_Start(void)
{
   // Initialize TIM4 for Motors PWM Output
   HAL_TIM_PWM_Start(&g_htim1_motor, TIM_CHANNEL_1);
   HAL_TIM_PWM_Start(&g_htim1_motor, TIM_CHANNEL_2);
   HAL_TIM_PWM_Start(&g_htim1_motor, TIM_CHANNEL_3);
   HAL_TIM_PWM_Start(&g_htim1_motor, TIM_CHANNEL_4);
}

// ***************************************************************************
// Fuction      : Board_Driver_Init()
// Description  : 
// 
//
// ***************************************************************************
void Board_Driver_Init()
{
   BSP_TIM1_Init();
   BSP_TIM1_Start();
}

