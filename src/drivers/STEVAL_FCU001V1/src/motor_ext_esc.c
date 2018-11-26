/*
 * File: motor_ext_esc.c
 *
 * Written by duvallee in 2018
 *
*/
#include "main.h"
#include "motor_ext_esc.h"

// ---------------------------------------------------------------------------
#if 0
#undef MOTOR_EXT_DC
#define MOTOR_EXT_ESC
#else
#define MOTOR_EXT_DC
#undef MOTOR_EXT_ESC
#endif

// ---------------------------------------------------------------------------
static TIM_HandleTypeDef g_htim2_motor;
static MotorExtControlTypeDef g_motor_ext_control;


// ***************************************************************************
// Fuction      : Motor_Ext_Init()
// Description  : 
// 
//
// ***************************************************************************
void Motor_Ext_Init(void)
{
   TIM_ClockConfigTypeDef sClockSourceConfig;
   TIM_MasterConfigTypeDef sMasterConfig;
   TIM_OC_InitTypeDef sConfigOC;

   g_motor_ext_control.motor1_pwm                        = MOTOR_EXT_MIN_PWM_VALUE;
   g_motor_ext_control.motor2_pwm                        = MOTOR_EXT_MIN_PWM_VALUE;
   g_motor_ext_control.motor3_pwm                        = MOTOR_EXT_MIN_PWM_VALUE;
   g_motor_ext_control.motor4_pwm                        = MOTOR_EXT_MIN_PWM_VALUE;

   g_htim2_motor.Instance                                = TIM2;
#ifdef MOTOR_EXT_DC
   g_htim2_motor.Init.Prescaler                          = 84;                                  /* DC motor configuration - Freq 494Hz*/
   g_htim2_motor.Init.CounterMode                        = TIM_COUNTERMODE_UP;
   g_htim2_motor.Init.Period                             = 1999;
#endif

#ifdef MOTOR_EXT_ESC
   g_htim2_motor.Init.Prescaler                          = 100;                                 /* ESC motor configuration - Freq 400Hz*/
   g_htim2_motor.Init.CounterMode                        = TIM_COUNTERMODE_UP;
   g_htim2_motor.Init.Period                             = 2075;
#endif
                     
   g_htim2_motor.Init.ClockDivision                      = TIM_CLOCKDIVISION_DIV1;
   if (HAL_TIM_Base_Init(&g_htim2_motor) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   sClockSourceConfig.ClockSource                        = TIM_CLOCKSOURCE_INTERNAL;
   if (HAL_TIM_ConfigClockSource(&g_htim2_motor, &sClockSourceConfig) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   if (HAL_TIM_PWM_Init(&g_htim2_motor) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   sMasterConfig.MasterOutputTrigger                     = TIM_TRGO_RESET;
   sMasterConfig.MasterSlaveMode                         = TIM_MASTERSLAVEMODE_DISABLE;
   HAL_TIMEx_MasterConfigSynchronization(&g_htim2_motor, &sMasterConfig);

   sConfigOC.OCMode                                      = TIM_OCMODE_PWM1;
   sConfigOC.Pulse                                       = 0;
   sConfigOC.OCPolarity                                  = TIM_OCPOLARITY_HIGH;
   sConfigOC.OCFastMode                                  = TIM_OCFAST_DISABLE;
   HAL_TIM_PWM_ConfigChannel(&g_htim2_motor, &sConfigOC, TIM_CHANNEL_1);
   HAL_TIM_PWM_ConfigChannel(&g_htim2_motor, &sConfigOC, TIM_CHANNEL_2);
   HAL_TIM_PWM_ConfigChannel(&g_htim2_motor, &sConfigOC, TIM_CHANNEL_3);
   HAL_TIM_PWM_ConfigChannel(&g_htim2_motor, &sConfigOC, TIM_CHANNEL_4);
}

// ***************************************************************************
// Fuction      : Motor_Ext_Start()
// Description  : 
// 
//
// ***************************************************************************
void Motor_Ext_Start(void)
{
   // Initialize TIM4 for Motors PWM Output
   HAL_TIM_PWM_Start(&g_htim2_motor, TIM_CHANNEL_1);
   HAL_TIM_PWM_Start(&g_htim2_motor, TIM_CHANNEL_2);
   HAL_TIM_PWM_Start(&g_htim2_motor, TIM_CHANNEL_3);
   HAL_TIM_PWM_Start(&g_htim2_motor, TIM_CHANNEL_4);
}


// ***************************************************************************
// Fuction      : Set_Motor_Ext_Pwm()
// Description  : 
//                Setup the driving power for 4 motors. p1~p4 data range is 0~1999, which equals
//                to 0~100% duty cycle (for DC motor configuration)
//
// ***************************************************************************
int Set_Motor_Ext_Pwm(MotorExtControlTypeDef* pmotor_ext_control)
{
   if (pmotor_ext_control->motor1_pwm > MOTOR_EXT_MAX_PWM_VALUE || pmotor_ext_control->motor1_pwm < MOTOR_EXT_MIN_PWM_VALUE ||
       pmotor_ext_control->motor2_pwm > MOTOR_EXT_MAX_PWM_VALUE || pmotor_ext_control->motor2_pwm < MOTOR_EXT_MIN_PWM_VALUE ||
       pmotor_ext_control->motor3_pwm > MOTOR_EXT_MAX_PWM_VALUE || pmotor_ext_control->motor3_pwm < MOTOR_EXT_MIN_PWM_VALUE ||
       pmotor_ext_control->motor4_pwm > MOTOR_EXT_MAX_PWM_VALUE || pmotor_ext_control->motor4_pwm < MOTOR_EXT_MIN_PWM_VALUE)
   {
      return -1;
   }

   g_motor_ext_control.motor1_pwm                        = pmotor_ext_control->motor1_pwm;
   g_motor_ext_control.motor2_pwm                        = pmotor_ext_control->motor2_pwm;
   g_motor_ext_control.motor3_pwm                        = pmotor_ext_control->motor3_pwm;
   g_motor_ext_control.motor4_pwm                        = pmotor_ext_control->motor4_pwm;
   return 0;
}



