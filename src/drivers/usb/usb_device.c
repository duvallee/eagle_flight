/*
 * File: usb_device.c
 *
 * Written by duvallee in 2018
 *
 */
#include "main.h"
#include "usbd_core.h"

#if defined(USE_USB_CDC_DEVICE)
#include "usbd_cdc.h"
#endif
#if defined(USE_USB_BULK_DEVICE)
#include "usbd_bulk.h"
#endif

#include "usbd_desc.h"

#if defined(USE_USB_CDC_DEVICE)
#include "usbd_cdc_interface.h"
#endif
#if defined(USE_USB_BULK_DEVICE)
#include "usbd_bulk_interface.h"
#endif
#include "usb_device.h"

// ===============================================================================
USBD_HandleTypeDef gUSBD_Device;

#if defined(USE_USB_CDC_DEVICE)
extern USBD_CDC_ItfTypeDef USBD_CDC_fops;
#endif

#if defined(USE_USB_BULK_DEVICE)
extern USBD_BULK_ItfTypeDef USBD_Interface_fops;
#endif

byte g_usb_recive_buffer[USB_MAX_RECEIVE_BUFFER_SIZE];
RING_BUFFER g_usb_ring_buffer[1]                         = {0, };

#if defined(RTOS_FREERTOS)
SemaphoreHandle_t g_usb_read_Semaphore                   = NULL;

/* --------------------------------------------------------------------------
 * Name : get_usb_device_semaphore()
 *
 *
 * -------------------------------------------------------------------------- */
osSemaphoreId get_usb_device_semaphore(void)
{
   return g_usb_read_Semaphore;
}
#endif

/* --------------------------------------------------------------------------
 * Name : usb_device_init()
 *
 *
 * -------------------------------------------------------------------------- */
int usb_device_init(void)
{
#if defined(USE_USB_CDC_DEVICE)
   // Init Device Library
   USBD_Init(&gUSBD_Device, &VCP_Desc, 0);

   // Add Supported Class
   USBD_RegisterClass(&gUSBD_Device, USBD_CDC_CLASS);

   // Add CDC Interface Class
   USBD_CDC_RegisterInterface(&gUSBD_Device, &USBD_CDC_fops);

   // Start Device Process
   USBD_Start(&gUSBD_Device);

#if defined(STM32H7xx)
//   HAL_PWREx_EnableUSBVoltageDetector();
#endif
#endif

#if defined(USE_USB_BULK_DEVICE)
   // Init Device Library
   USBD_Init(&gUSBD_Device, &BULK_Desc, 0);

   // Add Supported Class
   USBD_RegisterClass(&gUSBD_Device, &USBD_BULK);

   // Add CDC Interface Class
   USBD_BULK_RegisterInterface(&gUSBD_Device, &USBD_Interface_fops);

   // Start Device Process
   USBD_Start(&gUSBD_Device);

#if defined(STM32H7xx)
   HAL_PWREx_EnableUSBVoltageDetector();
#endif
#endif

   // initialize ring buffer for usb
   memset(g_usb_recive_buffer, 0, sizeof(g_usb_recive_buffer));
   initRingBuffer(g_usb_ring_buffer, g_usb_recive_buffer, sizeof(g_usb_recive_buffer));

#if defined(RTOS_FREERTOS)
   // create a binary semaphore used for informing ethernetif of frame reception
   g_usb_read_Semaphore                                  = xSemaphoreCreateBinary();
#endif

   return 0;
}

/* --------------------------------------------------------------------------
 * Name : usb_read()
 *
 *
 * -------------------------------------------------------------------------- */
void usb_read(uint8_t* Buf, uint16_t Len)
{
#if defined(RTOS_FREERTOS)
   UBaseType_t uxSavedInterruptStatus;
   BaseType_t xHigherPriorityTaskWoken                   = pdFALSE;

   uxSavedInterruptStatus                                = taskENTER_CRITICAL_FROM_ISR();
   if (writeRingBuffer(g_usb_ring_buffer, (byte*) Buf, (int) Len) < -1)
   {
      debug_output_error("buffer overrun : %d(required), %d(free) !!!", Len, getRingBufferFreeSize(g_usb_ring_buffer));
      return;
   }
   taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);

   if (g_usb_read_Semaphore != NULL)
   {
      xSemaphoreGiveFromISR(g_usb_read_Semaphore, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
   }
#endif
}

/* --------------------------------------------------------------------------
 * Name : usb_get_data()
 *
 *
 * -------------------------------------------------------------------------- */
int usb_get_data(uint8_t* Buf, uint16_t Len)
{
   return (readRingBuffer(g_usb_ring_buffer, Buf, Len));
}

/* --------------------------------------------------------------------------
 * Name : usb_write()
 *
 *
 * -------------------------------------------------------------------------- */
int usb_write(uint8_t* Buf, uint16_t Len)
{
#if defined(USE_USB_CDC_DEVICE)
   CDC_Itf_Transmitter(Buf, Len);
#endif

#if defined(USE_USB_BULK_DEVICE)
#endif
   return (int) Len;
}


