/*
 * File: scheduler.c
 *
 * Written by duvallee in 2018
 *
 */
#include "main.h"

// ---------------------------------------------------------------------------
#define MAX_SCHEDULER_COUNT                              1                          // max 8
#define SCHEDULER_FLAG_SIZE                              32                         // does not modified

#define RESET_EVENT_FLAG                                 0
#define SET_EVENT_FLAG                                   1

#define UNLOCK                                           0
#define LOCK                                             1

// ---------------------------------------------------------------------------
typedef struct SOFT_TIMER_STRUCT
{
   uint32_t cycle_ms;                                    // 
   uint32_t elapse_ms;
   uint8_t expire_count;
   volatile uint8_t lock;

   TIMER_FN fn;
} SOFT_TIMER;

// ---------------------------------------------------------------------------
typedef struct WAIT_EVENT_STRUCT
{
   uint8_t event;

   volatile uint8_t event_flag;
   volatile uint8_t duplicate_event_flag;
   volatile uint8_t lock;

   unsigned char event_name[WAIT_EVENT_NAME_LEN];

   uint8_t status;
   WAIT_EVENT_FN fn;

   uint16_t wait_ms;
   uint32_t elapse_ms;
} WAIT_EVENT;

// ---------------------------------------------------------------------------
typedef struct SCHEDULER_STRUCT
{
   uint32_t sheduler_time_ms;

   uint32_t soft_timer_flag[MAX_SCHEDULER_COUNT];
   uint32_t event_flag[MAX_SCHEDULER_COUNT];

   SOFT_TIMER soft_timer[MAX_SCHEDULER_COUNT * SCHEDULER_FLAG_SIZE];
   WAIT_EVENT event[MAX_SCHEDULER_COUNT * SCHEDULER_FLAG_SIZE];
} SCHEDULER;

// ---------------------------------------------------------------------------
static SCHEDULER g_scheduler;

// ***************************************************************************
// Fuction      : HAL_SYSTICK_Callback()
// Description  : 
// 
//
// ***************************************************************************
void HAL_SYSTICK_Callback()
{
   uint32_t index                                        = 0;
   uint32_t bits                                         = 0;
   unsigned char i;
   g_scheduler.sheduler_time_ms++;

   for (i = 0; i < (MAX_SCHEDULER_COUNT * SCHEDULER_FLAG_SIZE); i++)
   {
      if (g_scheduler.soft_timer[i].cycle_ms != 0)
      {
         if (g_scheduler.soft_timer[i].cycle_ms <= g_scheduler.soft_timer[i].elapse_ms)
         {
             g_scheduler.soft_timer[i].elapse_ms         = 1;
            index                                        = i / SCHEDULER_FLAG_SIZE;
            bits                                         = i % SCHEDULER_FLAG_SIZE;
            g_scheduler.soft_timer_flag[index]           |= (0x1 << bits);
         }
         else
         {
             g_scheduler.soft_timer[i].elapse_ms++;
         }
      }
      if (g_scheduler.event[i].event != NO_EVENT_ID)
      {
         if (g_scheduler.event[i].event_flag == SET_EVENT_FLAG)
         {
            if (g_scheduler.event[i].wait_ms != 0)
            {
               if (g_scheduler.event[i].wait_ms <= g_scheduler.event[i].elapse_ms)
               {
                  g_scheduler.event[i].elapse_ms         = 1;
                  index                                  = i / SCHEDULER_FLAG_SIZE;
                  bits                                   = i % SCHEDULER_FLAG_SIZE;
                  g_scheduler.event_flag[index]          |= (0x1 << bits);
               }
               else
               {
                  g_scheduler.event[i].elapse_ms++;
               }
            }
            else
            {
               index                                     = i / SCHEDULER_FLAG_SIZE;
               bits                                      = i % SCHEDULER_FLAG_SIZE;
               g_scheduler.event_flag[index]             |= (0x1 << bits);
            }
         }
       }
   }

}

// ***************************************************************************
// Fuction      : scheduler_run()
// Description  : 
// 
//
// ***************************************************************************
void scheduler_run()
{
   uint32_t index                                        = 0;
   uint32_t bits                                         = 0;
   unsigned char i;
   for (i = 0; i < (MAX_SCHEDULER_COUNT * SCHEDULER_FLAG_SIZE); i++)
   {
      index                                              = i / SCHEDULER_FLAG_SIZE;
      bits                                               = i % SCHEDULER_FLAG_SIZE;
      if ((g_scheduler.event_flag[index] & (0x1 << bits)) != 0)
      {
         // call
         g_scheduler.event_flag[index]                   &= ~(0x1 << bits);
         
         g_scheduler.event[i].elapse_ms                  = 0;
         g_scheduler.event[i].lock                       = LOCK;
         (*g_scheduler.event[i].fn)(g_scheduler.event[i].status);
         g_scheduler.event[i].lock                       = UNLOCK;
      }
      if ((g_scheduler.soft_timer_flag[index] & (0x1 << bits)) != 0)
      {
         // call
         g_scheduler.soft_timer_flag[index]              &= ~(0x1 << bits);

         if (g_scheduler.soft_timer[i].expire_count != INFINIT_TIMER_COUNT)
         {
            g_scheduler.soft_timer[i].elapse_ms          = 0;
            g_scheduler.soft_timer[i].lock               = LOCK;
            (*g_scheduler.soft_timer[i].fn)(g_scheduler.sheduler_time_ms);
            g_scheduler.soft_timer[i].lock               = UNLOCK;
         }
         else
         {
            if (g_scheduler.soft_timer[i].expire_count == 0)
            {
               g_scheduler.soft_timer[i].cycle_ms        = 0;
               g_scheduler.soft_timer[i].fn              = NULL;
               break;
            }
            g_scheduler.soft_timer[i].expire_count--;
            g_scheduler.soft_timer[i].elapse_ms          = 0;
            g_scheduler.soft_timer[i].lock               = LOCK;
            (*g_scheduler.soft_timer[i].fn)(g_scheduler.sheduler_time_ms);
            g_scheduler.soft_timer[i].lock               = UNLOCK;
         }
      }
   }
}




// ***************************************************************************
// Fuction      : scheduler_init()
// Description  : 
// 
//
// ***************************************************************************
void scheduler_init()
{
   unsigned char i;

   memset(&g_scheduler, 0, sizeof(g_scheduler));
   for (i = 0; i < (MAX_SCHEDULER_COUNT * SCHEDULER_FLAG_SIZE); i++)
   {
      g_scheduler.event[i].event                         = NO_EVENT_ID;
      g_scheduler.event[i].event_flag                    = RESET_EVENT_FLAG;
      g_scheduler.event[i].duplicate_event_flag          = RESET_EVENT_FLAG;
      g_scheduler.event[i].lock                          = UNLOCK;
      memset(g_scheduler.event[i].event_name, 0, sizeof(g_scheduler.event[i].event_name));
      g_scheduler.event[i].status                        = 0;
      g_scheduler.event[i].fn                            = NULL;
      g_scheduler.event[i].wait_ms                       = 0;
      g_scheduler.event[i].elapse_ms                     = 0;
   }
}


// ***************************************************************************
// Fuction      : add_timer()
// Description  : 
// 
//
// ***************************************************************************
int add_timer(uint32_t timer_ms, uint8_t count, TIMER_FN fn)
{
   unsigned char i;
   for (i = 0; i < (MAX_SCHEDULER_COUNT * SCHEDULER_FLAG_SIZE); i++)
   {
      if (g_scheduler.soft_timer[i].fn == NULL)
      {
         g_scheduler.soft_timer[i].cycle_ms              = timer_ms;
         g_scheduler.soft_timer[i].elapse_ms             = 0;
         if (count == 0)
         {
            g_scheduler.soft_timer[i].expire_count       = INFINIT_TIMER_COUNT;
         }
         else
         {
            g_scheduler.soft_timer[i].expire_count       = count;
         }
         g_scheduler.soft_timer[i].lock                  = UNLOCK;
         g_scheduler.soft_timer[i].fn                    = fn;
         return 0;
      }
   }
   return -1;
}

// ***************************************************************************
// Fuction      : add_event()
// Description  : 
// 
//
// ***************************************************************************
uint8_t add_event(uint16_t wait_ms, uint8_t status, WAIT_EVENT_FN fn, char* event_name)
{
   unsigned char i;
   for (i = 0; i < (MAX_SCHEDULER_COUNT * SCHEDULER_FLAG_SIZE); i++)
   {
      if (g_scheduler.event[i].event == NO_EVENT_ID)
      {
         g_scheduler.event[i].event                      = i;
         g_scheduler.event[i].event_flag                 = RESET_EVENT_FLAG;
         g_scheduler.event[i].duplicate_event_flag       = RESET_EVENT_FLAG;
         g_scheduler.event[i].lock                       = UNLOCK;
         if (event_name != NULL && strlen(event_name) < WAIT_EVENT_NAME_LEN)
         {
            memcpy(g_scheduler.event[i].event_name, event_name, strlen(event_name));
         }
         else
         {
            memset(g_scheduler.event[i].event_name, 0, sizeof(g_scheduler.event[i].event_name));
         }
         g_scheduler.event[i].status                     = status;
         g_scheduler.event[i].fn                         = fn;
         g_scheduler.event[i].wait_ms                    = wait_ms;
         g_scheduler.event[i].elapse_ms                  = 0;
         return (g_scheduler.event[i].event);
      }
   }
   return NO_EVENT_ID;
}


// ***************************************************************************
// Fuction      : set_event()
// Description  : 
// 
//
// ***************************************************************************
uint8_t set_event(uint8_t event, uint8_t status)
{
   unsigned char i;
   for (i = 0; i < (MAX_SCHEDULER_COUNT * SCHEDULER_FLAG_SIZE); i++)
   {
      if (g_scheduler.event[i].event == event)
      {
         g_scheduler.event[i].event_flag                 = SET_EVENT_FLAG;
         g_scheduler.event[i].status                     = status;
         return (g_scheduler.event[i].event);
      }
   }
   return NO_EVENT_ID;
}

// ***************************************************************************
// Fuction      : set_event_name()
// Description  : 
// 
//
// ***************************************************************************
uint8_t set_event_name(char* event_name, uint8_t status)
{
   unsigned char i;

   if (event_name == NULL || strlen(event_name) > WAIT_EVENT_NAME_LEN)
   {
      return NO_EVENT_ID;
   }

   for (i = 0; i < (MAX_SCHEDULER_COUNT * SCHEDULER_FLAG_SIZE); i++)
   {
      if (memcmp(g_scheduler.event[i].event_name, event_name, strlen(event_name)) == 0)
      {
         g_scheduler.event[i].event_flag                 = SET_EVENT_FLAG;
         g_scheduler.event[i].status                     = status;
         return (g_scheduler.event[i].event);
      }
   }
   return NO_EVENT_ID;
}


// ***************************************************************************
// Fuction      : get_event_status()
// Description  : 
// 
//
// ***************************************************************************
uint8_t get_event_status(uint8_t event)
{
   unsigned char i;
   for (i = 0; i < (MAX_SCHEDULER_COUNT * SCHEDULER_FLAG_SIZE); i++)
   {
      if (g_scheduler.event[i].event == event)
      {
         return (g_scheduler.event[i].status);
      }
   }
   return NO_EVENT_ID;
}

// ***************************************************************************
// Fuction      : get_event_name_status()
// Description  : 
// 
//
// ***************************************************************************
uint8_t get_event_name_status(char* event_name)
{
   unsigned char i;

   if (event_name == NULL || strlen(event_name) > WAIT_EVENT_NAME_LEN)
   {
      return NO_EVENT_ID;
   }

   for (i = 0; i < (MAX_SCHEDULER_COUNT * SCHEDULER_FLAG_SIZE); i++)
   {
      if (memcmp(g_scheduler.event[i].event_name, event_name, strlen(event_name)) == 0)
      {
         return (g_scheduler.event[i].status);
      }
   }
   return NO_EVENT_ID;
}


