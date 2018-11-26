/*
 * File: usbd_conf.c
 *
 * Written by duvallee.lee in 2018
 *
 */
#include "main.h"
#include "usbd_core.h"

// ===============================================================================
PCD_HandleTypeDef g_usb_pcd;


// *******************************************************************************
// Interrupt Handler
// *******************************************************************************
/* --------------------------------------------------------------------------
 * Name : USB_IRQHandler()
 *
 *
 * -------------------------------------------------------------------------- */
void USB_IRQHandler()
{
   HAL_PCD_IRQHandler(&g_usb_pcd);
}

// *******************************************************************************
// LL Driver Callbacks (PCD -> USB Device Library)
// *******************************************************************************

/* --------------------------------------------------------------------------
 * Name : HAL_PCD_SetupStageCallback()
 *
 * @brief  SetupStage callback.
 * @param  hpcd: PCD handle
 * @retval None
 * -------------------------------------------------------------------------- */
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef* hpcd)
{
   USBD_LL_SetupStage(hpcd->pData, (uint8_t *) hpcd->Setup);
}

/* --------------------------------------------------------------------------
 * Name : HAL_PCD_DataOutStageCallback()
 *
 * @brief  DataOut Stage callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 * -------------------------------------------------------------------------- */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef* hpcd, uint8_t epnum)
{
   USBD_LL_DataOutStage(hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

/* --------------------------------------------------------------------------
 * Name : HAL_PCD_DataInStageCallback()
 *
 * @brief  DataIn Stage callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 * -------------------------------------------------------------------------- */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef* hpcd, uint8_t epnum)
{
   USBD_LL_DataInStage(hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

/* --------------------------------------------------------------------------
 * Name : HAL_PCD_SOFCallback()
 *
 * @brief  SOF callback.
 * @param  hpcd: PCD handle
 * @retval None
 * -------------------------------------------------------------------------- */
void HAL_PCD_SOFCallback(PCD_HandleTypeDef* hpcd)
{
   USBD_LL_SOF(hpcd->pData);
}

/* --------------------------------------------------------------------------
 * Name : HAL_PCD_ResetCallback()
 *
 * @brief  Reset callback.
 * @param  hpcd: PCD handle
 * @retval None
 * -------------------------------------------------------------------------- */
void HAL_PCD_ResetCallback(PCD_HandleTypeDef* hpcd)
{
   USBD_SpeedTypeDef speed                               = USBD_SPEED_FULL;

   // Set USB Current Speed
   switch (hpcd->Init.speed)
   {
      case PCD_SPEED_HIGH :
         speed                                           = USBD_SPEED_HIGH;
         break;

      case PCD_SPEED_FULL :
         speed                                           = USBD_SPEED_FULL;
         break;

      default :
         speed                                           = USBD_SPEED_FULL;
         break;
   }

   // Reset Device
   USBD_LL_Reset(hpcd->pData);
   USBD_LL_SetSpeed(hpcd->pData, speed);
}

/* --------------------------------------------------------------------------
 * Name : HAL_PCD_SuspendCallback()
 *
 * @brief  Suspend callback.
 * @param  hpcd: PCD handle
 * @retval None
 * -------------------------------------------------------------------------- */
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef* hpcd)
{
   USBD_LL_Suspend(hpcd->pData);
}

/* --------------------------------------------------------------------------
 * Name : HAL_PCD_ResumeCallback()
 *
 * @brief  Resume callback.
 * @param  hpcd: PCD handle
 * @retval None
 * -------------------------------------------------------------------------- */
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef* hpcd)
{
   USBD_LL_Resume(hpcd->pData);
}

/* --------------------------------------------------------------------------
 * Name : HAL_PCD_ISOOUTIncompleteCallback()
 *
 * @brief  ISOOUTIncomplete callback.
 * @param  hpcd: PCD handle 
 * @param  epnum: Endpoint Number
 * @retval None
 * -------------------------------------------------------------------------- */
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef* hpcd, uint8_t epnum)
{
   USBD_LL_IsoOUTIncomplete(hpcd->pData, epnum);
}

/* --------------------------------------------------------------------------
 * Name : HAL_PCD_ISOINIncompleteCallback()
 *
 * @brief  ISOINIncomplete callback.
 * @param  hpcd: PCD handle 
 * @param  epnum: Endpoint Number
 * @retval None
 * -------------------------------------------------------------------------- */
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef* hpcd, uint8_t epnum)
{
   USBD_LL_IsoINIncomplete(hpcd->pData, epnum);
}

/* --------------------------------------------------------------------------
 * Name : HAL_PCD_ConnectCallback()
 *
 * @brief  ConnectCallback callback.
 * @param  hpcd: PCD handle
 * @retval None
 * -------------------------------------------------------------------------- */
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef * hpcd)
{
   USBD_LL_DevConnected(hpcd->pData);
}

/* --------------------------------------------------------------------------
 * Name : HAL_PCD_DisconnectCallback()
 *
 * @brief  Disconnect callback.
 * @param  hpcd: PCD handle
 * @retval None
 * -------------------------------------------------------------------------- */
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef * hpcd)
{
   USBD_LL_DevDisconnected(hpcd->pData);
}


// *******************************************************************************
// LL Driver Interface (USB Device Library --> PCD)
// *******************************************************************************

/* --------------------------------------------------------------------------
 * Name : USBD_LL_Init()
 *
 * @brief  Initializes the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 * -------------------------------------------------------------------------- */
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef* pdev)
{
#if defined(USE_USB_FS)
   // Set LL Driver parameters

#if defined(STM32F4xx)
   g_usb_pcd.Instance                                    = USB_OTG_FS;
#elif defined(STM32F7xx)
   g_usb_pcd.Instance                                    = USB_OTG_FS;
#elif defined(STM32H7xx)
   g_usb_pcd.Instance                                    = USB2_OTG_FS;
#else
#error "Unknown definition !!!"
#endif
   g_usb_pcd.Init.dev_endpoints                          = 8;
   g_usb_pcd.Init.use_dedicated_ep1                      = 0;
   g_usb_pcd.Init.ep0_mps                                = 0x40;

   // Be aware that enabling DMA mode will result in data being sent only by
   // multiple of 4 packet sizes. This is due to the fact that USB DMA does not
   // allow sending data from non word-aligned addresses. For this specific
   // application, it is advised to not enable this option unless required.
   g_usb_pcd.Init.dma_enable                             = 0;
   g_usb_pcd.Init.low_power_enable                       = 0;
   g_usb_pcd.Init.lpm_enable                             = 0;
   g_usb_pcd.Init.phy_itface                             = PCD_PHY_EMBEDDED;
   g_usb_pcd.Init.Sof_enable                             = 0;
   g_usb_pcd.Init.speed                                  = PCD_SPEED_FULL;
   g_usb_pcd.Init.vbus_sensing_enable                    = 0;

   // Link The driver to the stack
   g_usb_pcd.pData                                       = pdev;
   pdev->pData                                           = &g_usb_pcd;

   // Initialize LL Driver
   if (HAL_PCD_Init(&g_usb_pcd) != HAL_OK)
   {
      debug_output_error("HAL_PCD_Init() failed \r\n");
      return USBD_FAIL;
   }

   HAL_PCDEx_SetRxFiFo(&g_usb_pcd, 0x40);
   HAL_PCDEx_SetTxFiFo(&g_usb_pcd, 0, 0x40);
   HAL_PCDEx_SetTxFiFo(&g_usb_pcd, 1, 0x80);
#endif

#if defined(USE_USB_HS)
   // Set LL Driver parameters
   g_usb_pcd.Instance                                    = USB1_OTG_HS;
   g_usb_pcd.Init.dev_endpoints                          = 8;
   g_usb_pcd.Init.use_dedicated_ep1                      = 0;
   g_usb_pcd.Init.ep0_mps                                = 0x40;

   // Be aware that enabling DMA mode will result in data being sent only by
   // multiple of 4 packet sizes. This is due to the fact that USB DMA does not
   // allow sending data from non word-aligned addresses. For this specific
   // application, it is advised to not enable this option unless required.
   g_usb_pcd.Init.dma_enable                             = 0;
   g_usb_pcd.Init.low_power_enable                       = 0;
   g_usb_pcd.Init.lpm_enable                             = 0;
   g_usb_pcd.Init.phy_itface                             = PCD_PHY_ULPI;
   g_usb_pcd.Init.Sof_enable                             = 0;
   g_usb_pcd.Init.speed                                  = PCD_SPEED_HIGH;
   g_usb_pcd.Init.vbus_sensing_enable                    = 0;

   // Link The driver to the stack
   g_usb_pcd.pData                                       = pdev;
   pdev->pData                                           = &g_usb_pcd;

   // Initialize LL Driver
   if (HAL_PCD_Init(&g_usb_pcd) != HAL_OK)
   {
      debug_output_error("HAL_PCD_Init() failed \r\n");
      return USBD_FAIL;
   }

   HAL_PCDEx_SetRxFiFo(&g_usb_pcd, 0x200);
   HAL_PCDEx_SetTxFiFo(&g_usb_pcd, 0, 0x80);
   HAL_PCDEx_SetTxFiFo(&g_usb_pcd, 1, 0x174);
#endif

   return USBD_OK;
}

/* --------------------------------------------------------------------------
 * Name : USBD_LL_DeInit()
 *
 * @brief  De-Initializes the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 * -------------------------------------------------------------------------- */
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef* pdev)
{
   HAL_PCD_DeInit(pdev->pData);
   return USBD_OK;
}

/* --------------------------------------------------------------------------
 * Name : USBD_LL_Start()
 *
 * @brief  Starts the Low Level portion of the Device driver. 
 * @param  pdev: Device handle
 * @retval USBD Status
 * -------------------------------------------------------------------------- */
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef* pdev)
{
   HAL_PCD_Start(pdev->pData);
   return USBD_OK;
}

/* --------------------------------------------------------------------------
 * Name : USBD_LL_Stop()
 *
 * @brief  Stops the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 * -------------------------------------------------------------------------- */
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef * pdev)
{
   HAL_PCD_Stop(pdev->pData);
   return USBD_OK;
}

/* --------------------------------------------------------------------------
 * Name : USBD_LL_OpenEP()
 *
 * @brief  Opens an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  ep_type: Endpoint Type
 * @param  ep_mps: Endpoint Max Packet Size
 * @retval USBD Status
 * -------------------------------------------------------------------------- */
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps)
{
   HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);
   return USBD_OK;
}

/* --------------------------------------------------------------------------
 * Name : USBD_LL_CloseEP()
 *
 * @brief  Closes an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 * -------------------------------------------------------------------------- */
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr)
{
   HAL_PCD_EP_Close(pdev->pData, ep_addr);
   return USBD_OK;
}

/* --------------------------------------------------------------------------
 * Name : USBD_LL_FlushEP()
 *
 * @brief  Flushes an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 * -------------------------------------------------------------------------- */
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr)
{
   HAL_PCD_EP_Flush(pdev->pData, ep_addr);
   return USBD_OK;
}

/* --------------------------------------------------------------------------
 * Name : USBD_LL_StallEP()
 *
 * @brief  Sets a Stall condition on an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 * -------------------------------------------------------------------------- */
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr)
{
   HAL_PCD_EP_SetStall(pdev->pData, ep_addr);
   return USBD_OK;
}

/* --------------------------------------------------------------------------
 * Name : USBD_LL_ClearStallEP()
 *
 * @brief  Clears a Stall condition on an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 * -------------------------------------------------------------------------- */
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr)
{
   HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);
   return USBD_OK;
}

/* --------------------------------------------------------------------------
 * Name : USBD_LL_IsStallEP()
 *
 * @brief  Returns Stall condition.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval Stall (1: Yes, 0: No)
 * -------------------------------------------------------------------------- */
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr)
{
   PCD_HandleTypeDef *hpcd                               = pdev->pData;

   if ((ep_addr & 0x80) == 0x80)
   {
      return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
   }
   else
   {
      return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
   }
}

/* --------------------------------------------------------------------------
 * Name : USBD_LL_SetUSBAddress()
 *
 * @brief  Assigns a USB address to the device.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 * -------------------------------------------------------------------------- */
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef* pdev, uint8_t dev_addr)
{
   HAL_PCD_SetAddress(pdev->pData, dev_addr);
   return USBD_OK;
}

/* --------------------------------------------------------------------------
 * Name : USBD_LL_Transmit()
 *
 * @brief  Transmits data over an endpoint.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  pbuf: Pointer to data to be sent
 * @param  size: Data size    
 * @retval USBD Status
 * -------------------------------------------------------------------------- */
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef* pdev, uint8_t ep_addr, uint8_t* pbuf, uint16_t size)
{
   // Get the packet total length
   pdev->ep_in[ep_addr & 0x7F].total_length              = size;
   HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);
   return USBD_OK;
}

/* --------------------------------------------------------------------------
 * Name : USBD_LL_PrepareReceive()
 *
 * @brief  Prepares an endpoint for reception.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  pbuf: Pointer to data to be received
 * @param  size: Data size
 * @retval USBD Status
 * -------------------------------------------------------------------------- */
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef* pdev, uint8_t ep_addr, uint8_t* pbuf, uint16_t size)
{
   HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);
   return USBD_OK;
}

/* --------------------------------------------------------------------------
 * Name : USBD_LL_GetRxDataSize()
 *
 * @brief  Returns the last transferred packet size.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval Received Data Size
 * -------------------------------------------------------------------------- */
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef* pdev, uint8_t ep_addr)
{
   return HAL_PCD_EP_GetRxCount(pdev->pData, ep_addr);
}

/* --------------------------------------------------------------------------
 * Name : USBD_LL_Delay()
 *
 * @brief  Delays routine for the USB Device Library.
 * @param  Delay: Delay in ms
 * @retval None
 * -------------------------------------------------------------------------- */
void USBD_LL_Delay(uint32_t Delay)
{
   HAL_Delay(Delay);
}



