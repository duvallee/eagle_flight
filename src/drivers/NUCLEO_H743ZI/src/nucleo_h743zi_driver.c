/*
 * File: nucleo_h743zi_driver.c
 *
 * Written by duvallee in 2018
 *
 */
#include "main.h"
#include "nucleo_h743zi_driver.h"

#if defined(TRAXXAS_LATRAX_RECEIVER)
#include "traxxas_latrax_receiver.h"
#endif

// --------------------------------------------------------------------------
#define APB1_BUS_TIMERCLOCK                              200000000
#define TIMER_1_CLOCK                                    10000000

// --------------------------------------------------------------------------
static TIM_HandleTypeDef g_htim1_timer;

/* --------------------------------------------------------------------------
 * Name : BSP_TIM1_Init()
 *        
 *
 * -------------------------------------------------------------------------- */
static void BSP_TIM1_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct;
   TIM_ClockConfigTypeDef sClockSourceConfig             = {0};
//   TIM_MasterConfigTypeDef sMasterConfig                 = {0};
//   TIM_IC_InitTypeDef sICConfig                          = {0};

   g_htim1_timer.Instance                                = TIM1;                                         // APB1 Bus(TIMER Clock : 200MHz)
   g_htim1_timer.Init.Prescaler                          = (APB1_BUS_TIMERCLOCK / TIMER_1_CLOCK) - 1;    // Timer1 Clock : 200MHz / (19 + 1) = 10MHz
   g_htim1_timer.Init.CounterMode                        = TIM_COUNTERMODE_UP;
   g_htim1_timer.Init.Period                             = 9;                                            // 10 : 1us
   g_htim1_timer.Init.ClockDivision                      = TIM_CLOCKDIVISION_DIV1;

   if (HAL_TIM_Base_Init(&g_htim1_timer) != HAL_OK)
   {
      _Error_Handler(__FILE__, __LINE__);
   }

   sClockSourceConfig.ClockSource                        = TIM_CLOCKSOURCE_INTERNAL;
   if (HAL_TIM_ConfigClockSource(&g_htim1_timer, &sClockSourceConfig) != HAL_OK)
   {
     _Error_Handler(__FILE__, __LINE__);
   }

//   sMasterConfig.MasterOutputTrigger                     = TIM_TRGO_RESET;
//   sMasterConfig.MasterOutputTrigger2                    = TIM_TRGO2_RESET;
//   sMasterConfig.MasterSlaveMode                         = TIM_MASTERSLAVEMODE_DISABLE;
//   if (HAL_TIMEx_MasterConfigSynchronization(&g_htim1_timer, &sMasterConfig) != HAL_OK)
//   {
//     _Error_Handler(__FILE__, __LINE__);
//   }

   if(HAL_TIM_Base_Start_IT(&g_htim1_timer) != HAL_OK)
   {
      // Starting Error
      _Error_Handler(__FILE__, __LINE__);
   }

   HAL_NVIC_SetPriority(TIM1_UP_IRQn, 8, 0);
   HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);


   __HAL_RCC_GPIOE_CLK_ENABLE();

   // TIMER 1 CHANNEL 1, 2
   GPIO_InitStruct.Pin                                   = GPIO_PIN_9 | GPIO_PIN_11;
   GPIO_InitStruct.Mode                                  = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pull                                  = GPIO_NOPULL;
   GPIO_InitStruct.Speed                                 = GPIO_SPEED_FREQ_HIGH;
   GPIO_InitStruct.Alternate                             = 0;
   HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

// ***************************************************************************
// Fuction      : Board_Driver_Init()
// Description  : 
// 
//
// ***************************************************************************
void test_task(void const* argument);

void Board_Driver_Init()
{
#if defined(TRAXXAS_LATRAX_RECEIVER)
   BSP_TIM1_Init();
   traxxas_latrax_receiver_init((void *) &g_htim1_timer);
#endif

   // --------------------------------------------------------------------------
   /* Thread definition for tcp server */
   osThreadDef(test_task, test_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
   if (osThreadCreate(osThread(test_task), (void *) NULL) == NULL)
   {
      debug_output_error("Can't create thread : test_task !!! \r\n");
   }

}

/* --------------------------------------------------------------------------
 * Name : BSP_TIM1_CC_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void BSP_TIM1_CC_IRQHandler()
{
   HAL_TIM_IRQHandler(&g_htim1_timer);
}

/* --------------------------------------------------------------------------
 * Name : BSP_TIM1_UP_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
static volatile uint32_t channel_1_duty                  = 0;
static volatile uint32_t channel_2_duty                  = 0;

static volatile uint32_t channel_1_status                = 0;
static volatile uint32_t channel_1_count                 = 0;

static volatile uint32_t channel_2_status                = 0;
static volatile uint32_t channel_2_count                 = 0;

void BSP_TIM1_UP_IRQHandler()
{
   GPIO_PinState PinStatus                               = GPIO_PIN_RESET;
   HAL_TIM_IRQHandler(&g_htim1_timer);

   PinStatus                                             = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9);

//   debug_output_info(" %d \r\n", PinStatus);

   if (PinStatus != channel_1_status)
   {
      channel_1_status                                   = PinStatus;
      if (channel_1_status == GPIO_PIN_RESET)
      {
         channel_1_duty                                  = channel_1_count;
      }
      channel_1_count                                    = 0;
   }
   else
   {
      channel_1_count++;
   }

   PinStatus                                             = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11);
   if (PinStatus != channel_2_status)
   {
      channel_2_status                                   = PinStatus;
      if (channel_2_status == GPIO_PIN_RESET)
      {
         channel_2_duty                                  = channel_2_count;
      }
      channel_2_count                                    = 0;
   }
   else
   {
      channel_2_count++;
   }

}

void test_task(void const* argument)
{
   while (1)
   {
      osDelay(500);
      debug_output_info("CHANNEL 1 : %d, CHANNEL 2 : %d \r\n", channel_1_duty, channel_2_duty);
   }
}


