/*
 * File: motor.c
 *
 * Written by duvallee in 2018
 *
*/
#include "main.h"
#include "motor.h"

// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
static TIM_HandleTypeDef g_htim4_motor;
static MotorControlTypeDef g_motor_control;


// ***************************************************************************
// Fuction      : Motor_Init()
// Description  : 
// 
//
// ***************************************************************************
void Motor_Init(void)
{
   TIM_ClockConfigTypeDef sClockSourceConfig;
   TIM_MasterConfigTypeDef sMasterConfig;
   TIM_OC_InitTypeDef sConfigOC;

   g_motor_control.motor1_pwm                            = MOTOR_MIN_PWM_VALUE;
   g_motor_control.motor2_pwm                            = MOTOR_MIN_PWM_VALUE;
   g_motor_control.motor3_pwm                            = MOTOR_MIN_PWM_VALUE;
   g_motor_control.motor4_pwm                            = MOTOR_MIN_PWM_VALUE;

   g_htim4_motor.Instance                                = TIM4;
#ifdef MOTOR_DC
   g_htim4_motor.Init.Prescaler                          = 84;                                  /* DC motor configuration - Freq 494Hz*/
   g_htim4_motor.Init.CounterMode                        = TIM_COUNTERMODE_UP;
   g_htim4_motor.Init.Period                             = 1999;
#endif

#ifdef MOTOR_ESC
   g_htim4_motor.Init.Prescaler                          = 100;                                 /* ESC motor configuration - Freq 400Hz*/
   g_htim4_motor.Init.CounterMode                        = TIM_COUNTERMODE_UP;
   g_htim4_motor.Init.Period                             = 2075;
#endif
                     
   g_htim4_motor.Init.ClockDivision                      = TIM_CLOCKDIVISION_DIV1;
   if (HAL_TIM_Base_Init(&g_htim4_motor) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   sClockSourceConfig.ClockSource                        = TIM_CLOCKSOURCE_INTERNAL;
   if (HAL_TIM_ConfigClockSource(&g_htim4_motor, &sClockSourceConfig) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   if (HAL_TIM_PWM_Init(&g_htim4_motor) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   sMasterConfig.MasterOutputTrigger                     = TIM_TRGO_RESET;
   sMasterConfig.MasterSlaveMode                         = TIM_MASTERSLAVEMODE_DISABLE;
   HAL_TIMEx_MasterConfigSynchronization(&g_htim4_motor, &sMasterConfig);

   sConfigOC.OCMode                                      = TIM_OCMODE_PWM1;
   sConfigOC.Pulse                                       = 0;
   sConfigOC.OCPolarity                                  = TIM_OCPOLARITY_HIGH;
   sConfigOC.OCFastMode                                  = TIM_OCFAST_DISABLE;
   HAL_TIM_PWM_ConfigChannel(&g_htim4_motor, &sConfigOC, TIM_CHANNEL_1);
   HAL_TIM_PWM_ConfigChannel(&g_htim4_motor, &sConfigOC, TIM_CHANNEL_2);
   HAL_TIM_PWM_ConfigChannel(&g_htim4_motor, &sConfigOC, TIM_CHANNEL_3);
   HAL_TIM_PWM_ConfigChannel(&g_htim4_motor, &sConfigOC, TIM_CHANNEL_4);
}

// ***************************************************************************
// Fuction      : Motor_Start()
// Description  : 
// 
//
// ***************************************************************************
void Motor_Start(void)
{
   // Initialize TIM4 for Motors PWM Output
   HAL_TIM_PWM_Start(&g_htim4_motor, TIM_CHANNEL_1);
   HAL_TIM_PWM_Start(&g_htim4_motor, TIM_CHANNEL_2);
   HAL_TIM_PWM_Start(&g_htim4_motor, TIM_CHANNEL_3);
   HAL_TIM_PWM_Start(&g_htim4_motor, TIM_CHANNEL_4);
}


// ***************************************************************************
// Fuction      : Set_Motor_Pwm()
// Description  : 
//                Setup the driving power for 4 motors. p1~p4 data range is 0~1999, which equals
//                to 0~100% duty cycle (for DC motor configuration)
//
// ***************************************************************************
int Set_Motor_Pwm(MotorControlTypeDef* pmotor_control)
{
   if (pmotor_control->motor1_pwm > MOTOR_MAX_PWM_VALUE || pmotor_control->motor1_pwm < MOTOR_MIN_PWM_VALUE ||
       pmotor_control->motor2_pwm > MOTOR_MAX_PWM_VALUE || pmotor_control->motor2_pwm < MOTOR_MIN_PWM_VALUE ||
       pmotor_control->motor3_pwm > MOTOR_MAX_PWM_VALUE || pmotor_control->motor3_pwm < MOTOR_MIN_PWM_VALUE ||
       pmotor_control->motor4_pwm > MOTOR_MAX_PWM_VALUE || pmotor_control->motor4_pwm < MOTOR_MIN_PWM_VALUE)
   {
      return -1;
   }

   g_motor_control.motor1_pwm                            = pmotor_control->motor1_pwm;
   g_motor_control.motor2_pwm                            = pmotor_control->motor2_pwm;
   g_motor_control.motor3_pwm                            = pmotor_control->motor3_pwm;
   g_motor_control.motor4_pwm                            = pmotor_control->motor4_pwm;
   return 0;
}



