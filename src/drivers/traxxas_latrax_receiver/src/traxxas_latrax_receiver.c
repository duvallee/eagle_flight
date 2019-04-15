/*
 * File: traxxas_latrax_receiver.c
 *
 * Written by duvallee in 2019
 *
 */
#include "main.h"
#include "traxxas_latrax_receiver.h"

// ============================================================================
// static I2C_HandleTypeDef* g_i2c_handle                   = NULL;



/* --------------------------------------------------------------------------
 * Name : traxxas_latrax_receiver_init()
 *
 *
 * -------------------------------------------------------------------------- */
int traxxas_latrax_receiver_init(void* pTimerHandle)
{

//      debug_output_error("I2C Expander @0x%02X not detected \r\n", (int) I2cExpAddr0);
   debug_output_info("traxxas_latrax_receiver_init \r\n");


   return 0;
}



