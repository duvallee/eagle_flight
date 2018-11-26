/*
 * File: remote_controller.h
 *
 * Written by duvallee in 2018
 *
 */

#ifndef __REMOTE_CONTROLLER_H__
#define __REMOTE_CONTROLLER__
#ifdef __cplusplus
extern "C" {
#endif

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
void RemoteController_Init(void);

#ifdef __cplusplus
}
#endif

#endif      // __REMOTE_CONTROLLER__

