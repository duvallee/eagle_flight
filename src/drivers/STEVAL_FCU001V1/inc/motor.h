/*
 * File: motor.h
 *
 * Written by duvallee in 2018
 *
 */

#ifndef __MOTOR_H__
#define __MOTOR_H__
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
} MotorControlTypeDef;

#define MOTOR_MAX_PWM_VALUE                              1900.0f
#define MOTOR_MIN_PWM_VALUE                              0.0f

// --------------------------------------------------------------------------------
#define MOTOR_1_GPIO_B_6                                 GPIO_PIN_6
#define MOTOR_1_PORT                                     GPIOB

#define MOTOR_2_GPIO_B_7                                 GPIO_PIN_7
#define MOTOR_2_PORT                                     GPIOB

#define MOTOR_3_GPIO_B_8                                 GPIO_PIN_8
#define MOTOR_3_PORT                                     GPIOB

#define MOTOR_4_GPIO_B_9                                 GPIO_PIN_9
#define MOTOR_4_PORT                                     GPIOB

// --------------------------------------------------------------------------------
void Motor_Init(void);
void Motor_Start(void);
int Set_Motor_Pwm(MotorControlTypeDef* pmotor_control);

#ifdef __cplusplus
}
#endif

#endif      // __MOTOR_H__

