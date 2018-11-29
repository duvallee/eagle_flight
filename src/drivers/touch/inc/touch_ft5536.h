/*
 * File: touch_ft5536.h
 *
 * Written by duvallee in 2018
 *
 */


#ifndef  __TOUCH_FT5536_H__
#define  __TOUCH_FT5536_H__

// ============================================================================
#define TOUCH_FT5536_MAX_TOUCH                           5

typedef int (*FT5536_READ_FN)(uint16_t addr, uint16_t reg_addr, uint16_t reg_addr_size, uint8_t* pData, uint16_t DataSize);
typedef int (*FT5536_WRITE_FN)(uint16_t addr, uint16_t reg_addr, uint16_t reg_addr_size, uint8_t* pData, uint16_t DataSize);

typedef enum
{
   TOUCH_FT5536_TRIGGER_MODE_POLLING                     = 0,
   TOUCH_FT5536_TRIGGER_MODE_INTERRUPT                   = 1,
} TOUCH_FT5536_TRIGGER_MODE;

typedef struct
{
   uint8_t touch_action;

   uint16_t x_pos;
   uint16_t y_pos;

   uint8_t touch_weight;
   uint8_t touch_area;
} TOUCH_POINT_STRUCT;

typedef struct
{
   uint8_t touch_point_num;

   TOUCH_POINT_STRUCT touch_point[TOUCH_FT5536_MAX_TOUCH];

   uint8_t touch_gesture;
} TOUCH_EVENT_STRUCT;

typedef struct
{
   FT5536_READ_FN read_fn;
   FT5536_WRITE_FN write_fn;
} TOUCH_FT5536_STRUCT;


// ============================================================================
void touch_ft5536_init(FT5536_READ_FN, FT5536_WRITE_FN);
void touch_start(TOUCH_FT5536_TRIGGER_MODE trigger_mode);

// void touch_start(uint8_t trigger_mode);
// void touch_polling(void*);


#endif // __TOUCH_FT5536_H__

