/*
 * File: usbd_bulk_desc.h
 *
 * Written by duvallee.lee in 2018
 *
 */

#include "main.h"
#include "usbd_core.h"
#include "usbd_conf.h"

// ===============================================================================
#define USBD_VID                                            0x0483
#define USBD_PID                                            0x0401

#define USBD_LANGID_STRING                                  0x0409
#define USBD_MANUFACTURER_STRING                            "duvallee"
#define USBD_PRODUCT_STRING                                 "bootloader"
#define USBD_SERIALNUMBER_STRING                            "000000000001"
#define USBD_CONFIGURATION_STRING                           "Bulk Config"
#define USBD_INTERFACE_STRING                               "Bulk Interface (subset)"

// ===============================================================================
uint8_t *USBD_BULK_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_BULK_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_BULK_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_BULK_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_BULK_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_BULK_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_BULK_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);

// ===============================================================================
USBD_DescriptorsTypeDef BULK_Desc                        =
{
   USBD_BULK_DeviceDescriptor,
   USBD_BULK_LangIDStrDescriptor, 
   USBD_BULK_ManufacturerStrDescriptor,
   USBD_BULK_ProductStrDescriptor,
   USBD_BULK_SerialStrDescriptor,
   USBD_BULK_ConfigStrDescriptor,
   USBD_BULK_InterfaceStrDescriptor,
};

// ===============================================================================
// USB Standard Device Descriptor
__ALIGN_BEGIN uint8_t USBD_HS_DeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END =
{
   0x12,                                                 // bLength 
   USB_DESC_TYPE_DEVICE,                                 // bDescriptorType
   0x00,                                                 // bcdUSB
   0x02,
   0xff,                                                 // bDeviceClass
   0x00,                                                 // bDeviceSubClass
   0x00,                                                 // bDeviceProtocol
   USB_MAX_EP0_SIZE,                                     // MaxPacketSize
   LOBYTE(USBD_VID),                                     // idVendor
   HIBYTE(USBD_VID),                                     // idVendor
   LOBYTE(USBD_PID),                                     // idVendor
   HIBYTE(USBD_PID),                                     // idVendor
   0x00,                                                 // bcdDevice rel. 2.00
   0x02,
   USBD_IDX_MFC_STR,                                     // Index of manufacturer  string
   USBD_IDX_PRODUCT_STR,                                 // Index of product string
   USBD_IDX_SERIAL_STR,                                  // Index of serial number string
   1                                                     // bNumConfigurations
} ; 
// USB_DeviceDescriptor

// USB Standard Device Descriptor
__ALIGN_BEGIN uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END =
{
   USB_LEN_LANGID_STR_DESC,         
   USB_DESC_TYPE_STRING,       
   LOBYTE(USBD_LANGID_STRING),
   HIBYTE(USBD_LANGID_STRING), 
};
__ALIGN_BEGIN uint8_t USBD_StrDesc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;


/* --------------------------------------------------------------------------
 * Name : USBD_BULK_DeviceDescriptor()
 *
 *
 *
 * -------------------------------------------------------------------------- */
uint8_t* USBD_BULK_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t* length)
{
   *length                                               = sizeof(USBD_HS_DeviceDesc);
   return USBD_HS_DeviceDesc;
}

/* --------------------------------------------------------------------------
 * Name : USBD_BULK_LangIDStrDescriptor()
 *
 *
 *
 * -------------------------------------------------------------------------- */
uint8_t* USBD_BULK_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t* length)
{
   *length                                               = sizeof(USBD_LangIDDesc);  
   return USBD_LangIDDesc;
}

/* --------------------------------------------------------------------------
 * Name : USBD_HS_ProductStrDescriptor()
 *
 *
 *
 * -------------------------------------------------------------------------- */
uint8_t* USBD_BULK_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t* length)
{
   USBD_GetString((uint8_t *) USBD_PRODUCT_STRING, USBD_StrDesc, length);
   return USBD_StrDesc;
}

/* --------------------------------------------------------------------------
 * Name : USBD_HS_ManufacturerStrDescriptor()
 *
 *
 *
 * -------------------------------------------------------------------------- */
uint8_t* USBD_BULK_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed,  uint16_t* length)
{
   USBD_GetString((uint8_t *) USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
   return USBD_StrDesc;
}

/* --------------------------------------------------------------------------
 * Name : USBD_HS_SerialStrDescriptor()
 *
 *
 *
 * -------------------------------------------------------------------------- */
uint8_t* USBD_BULK_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t* length)
{
   USBD_GetString((uint8_t *) USBD_SERIALNUMBER_STRING, USBD_StrDesc, length);
   return USBD_StrDesc;
}

/* --------------------------------------------------------------------------
 * Name : USBD_HS_ConfigStrDescriptor()
 *
 *
 *
 * -------------------------------------------------------------------------- */
uint8_t* USBD_BULK_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t* length)
{
   USBD_GetString((uint8_t *) USBD_CONFIGURATION_STRING, USBD_StrDesc, length);
   return USBD_StrDesc;
}

/* --------------------------------------------------------------------------
 * Name : USBD_HS_InterfaceStrDescriptor()
 *
 *
 *
 * -------------------------------------------------------------------------- */
uint8_t* USBD_BULK_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
   USBD_GetString((uint8_t *) USBD_INTERFACE_STRING, USBD_StrDesc, length);
   return USBD_StrDesc;  
}


// ===============================================================================
