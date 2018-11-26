/*
 * File: usbd_bulk_interface.c
 *
 * Written by duvallee.lee in 2018
 *
 */
#include "main.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_bulk.h"
#include "usbd_bulk_interface.h"


// ==========================================================================
#if defined(USE_USB_HS)
#define APP_RX_DATA_SIZE                                 2048
#define APP_TX_DATA_SIZE                                 2048
#elif defined(USE_USB_FS)
#define APP_RX_DATA_SIZE                                 1024
#define APP_TX_DATA_SIZE                                 1024
#else
#error "Not defined USE_USB_HS or USE_USB_FS"
#endif

static uint8_t UserRxBuffer[APP_RX_DATA_SIZE];
static uint8_t UserTxBuffer[APP_TX_DATA_SIZE];

// ==========================================================================
extern USBD_HandleTypeDef gUSBD_Device;

static int8_t BULK_Init(void);
static int8_t BULK_DeInit(void);
static int8_t BULK_Receive(uint8_t* Buf, uint32_t *Len);

// ==========================================================================
USBD_BULK_ItfTypeDef USBD_Interface_fops                 = 
{
   BULK_Init,
   BULK_DeInit,
   BULK_Receive,
};

/* --------------------------------------------------------------------------
 * Name : BULK_Init()
 *
 *
 *
 * -------------------------------------------------------------------------- */
static int8_t BULK_Init(void)
{
   USBD_BULK_SetTxBuffer(&gUSBD_Device, UserTxBuffer, 0);
   USBD_BULK_SetRxBuffer(&gUSBD_Device, UserRxBuffer);

   return (USBD_OK);
}

/* --------------------------------------------------------------------------
 * Name : BULK_DeInit()
 *
 *
 *
 * -------------------------------------------------------------------------- */
static int8_t BULK_DeInit(void)
{
  return (USBD_OK);
}

/* --------------------------------------------------------------------------
 * Name : BULK_Receive()
 *
 *
 *
 * -------------------------------------------------------------------------- */
static int8_t BULK_Receive(uint8_t* Buf, uint32_t *Len)
{
   USBD_BULK_SetRxBuffer(&gUSBD_Device, UserRxBuffer);
   if (USBD_BULK_ReceivePacket(&gUSBD_Device) != USBD_OK)
   {
      return -1;
   }
   return 0;
}

/* --------------------------------------------------------------------------
 * Name : BULK_Transmit()
 *
 *
 *
 * -------------------------------------------------------------------------- */
enum BULK_TRANSMIT_STATUS BULK_Transmit(uint8_t* Buf, uint16_t Len)
{
   USBD_BULK_HandleTypeDef* hcdc                         = (USBD_BULK_HandleTypeDef*) (&gUSBD_Device)->pClassData;
   int wait_count                                        = 0;

   if (hcdc == NULL)
   {
      return BULK_TRANSMIT_ERROR_NULL_POINTER;
   }

   USBD_BULK_SetTxBuffer(&gUSBD_Device, Buf, Len);
   if (USBD_BULK_TransmitPacket(&gUSBD_Device) != USBD_OK)
   {
      return BULK_TRANSMIT_ERROR;
   }

   // blocking transmit -- need to wait until transmit is complete
   // move this up to stlink level

   while (((volatile uint32_t) hcdc->TxState))
   {
      HAL_Delay(10);
      // wait 3 Second ...
      if (wait_count > 300)
      {
         debug_output_error("BULK Transmitt Time-Out !!! \r\n");
         return BULK_TRANSMIT_ERROR_TIME_OUT;
      }
   }
   return BULK_TRANSMIT_SUCCESS;
}


