/*
 * File: remote_controller.c
 *
 * Written by duvallee in 2018
 *
*/
#include "main.h"
#include "remote_controller.h"

// ---------------------------------------------------------------------------
static TIM_HandleTypeDef g_htim2_remote_controller;


// ***************************************************************************
// Fuction      : RemoteController_Init()
// Description  : 
// 
//
// ***************************************************************************
void RemoteController_Init(void)
{
   TIM_ClockConfigTypeDef sClockSourceConfig;
   TIM_MasterConfigTypeDef sMasterConfig;
   TIM_IC_InitTypeDef sConfigIC;

   g_htim2_remote_controller.Instance                    = TIM2;
   g_htim2_remote_controller.Init.Prescaler              = 20;
   g_htim2_remote_controller.Init.CounterMode            = TIM_COUNTERMODE_UP;
   g_htim2_remote_controller.Init.Period                 = 32767;
                     
   g_htim2_remote_controller.Init.ClockDivision          = TIM_CLOCKDIVISION_DIV1;
   if (HAL_TIM_Base_Init(&g_htim2_remote_controller) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   sClockSourceConfig.ClockSource                        = TIM_CLOCKSOURCE_INTERNAL;
   if (HAL_TIM_ConfigClockSource(&g_htim2_remote_controller, &sClockSourceConfig) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   if (HAL_TIM_IC_Init(&g_htim2_remote_controller) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }


   sMasterConfig.MasterOutputTrigger                     = TIM_TRGO_RESET;
   sMasterConfig.MasterSlaveMode                         = TIM_MASTERSLAVEMODE_DISABLE;
   if (HAL_TIMEx_MasterConfigSynchronization(&g_htim2_remote_controller, &sMasterConfig) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   sConfigIC.ICPolarity                                  = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
   sConfigIC.ICSelection                                 = TIM_ICSELECTION_DIRECTTI;
   sConfigIC.ICPrescaler                                 = TIM_ICPSC_DIV1;
   sConfigIC.ICFilter                                    = 0;
   if (HAL_TIM_IC_ConfigChannel(&g_htim2_remote_controller, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   if (HAL_TIM_IC_ConfigChannel(&g_htim2_remote_controller, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   if (HAL_TIM_IC_ConfigChannel(&g_htim2_remote_controller, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   if (HAL_TIM_IC_ConfigChannel(&g_htim2_remote_controller, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }
}





