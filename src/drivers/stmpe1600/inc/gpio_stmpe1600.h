/*
 * File: gpio_stmpe1600.h
 *
 * Written by duvallee in 2018
 *
 */


#ifndef  __GPIO_STMPE1600_H__
#define  __GPIO_STMPE1600_H__
// ============================================================================
enum XNUCLEO53L1A1_dev_e
{
   XNUCLEO53L1A1_DEV_CENTER                              = 0,                    // !< center (built-in) vl053 device : 'c"
   XNUCLEO53L1A1_DEV_LEFT                                = 1,                    // !< left satellite device P21 header : 'l'
   XNUCLEO53L1A1_DEV_RIGHT                               = 2                     // !< Right satellite device P22 header : 'r'
};

// ============================================================================
void gpio_stmpe1600_init(void*, void*);
void gpio_stmpe1600_reset(enum XNUCLEO53L1A1_dev_e DevNo, int state);

int XNUCLEO53L1A1_SetDisplayString(const char *str);
#endif // __GPIO_STMPE1600_H__

