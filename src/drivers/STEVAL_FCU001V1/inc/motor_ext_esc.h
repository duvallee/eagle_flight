/*
 * File: motor_ext_esc.h
 *
 * Written by duvallee in 2018
 *
 */

#ifndef __MOTOR_EXT_ESC_H__
#define __MOTOR_EXT_ESC_H__
#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------------------
typedef struct
{
   float motor1_pwm;
   float motor2_pwm;
   float motor3_pwm;
   float motor4_pwm;
} MotorExtControlTypeDef;

#define MOTOR_EXT_MAX_PWM_VALUE                          1900.0f
#define MOTOR_EXT_MIN_PWM_VALUE                          0.0f

// --------------------------------------------------------------------------------
#define MOTOR_EXT_1_GPIO_A_0                             GPIO_PIN_0
#define MOTOR_EXT_1_PORT                                 GPIOA

#define MOTOR_EXT_2_GPIO_A_1                             GPIO_PIN_1
#define MOTOR_EXT_2_PORT                                 GPIOA

#define MOTOR_EXT_3_GPIO_A_2                             GPIO_PIN_2
#define MOTOR_EXT_3_PORT                                 GPIOA

#define MOTOR_EXT_4_GPIO_A_3                             GPIO_PIN_3
#define MOTOR_EXT_4_PORT                                 GPIOA

// --------------------------------------------------------------------------------
void Motor_Ext_Init(void);
void Motor_Ext_Start(void);
int Set_Motor_Ext_Pwm(MotorExtControlTypeDef* pmotor_ext_control);

#ifdef __cplusplus
}
#endif

#endif      // __MOTOR_EXT_ESC_H__

