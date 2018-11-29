/**
  ******************************************************************************
  * File Name          : ethernetif.c
  * Description        : This file provides code for the configuration
  *                      of the ethernetif.c MiddleWare.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "main.h"
#include "lwip/opt.h"

#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"
#include "ethernetif.h"
#include <string.h>

#include "cmsis_os.h"

/* Private define ------------------------------------------------------------*/
/* The time to block waiting for input. */
#define TIME_WAITING_FOR_INPUT                           (portMAX_DELAY)
/* Stack size of the interface thread */
#define INTERFACE_THREAD_STACK_SIZE                      (350)
/* Network interface name */
#define IFNAME0                                          's'
#define IFNAME1                                          't'

/* Private variables ---------------------------------------------------------*/

#if defined (__CC_ARM)
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] __attribute__((at(0x20010000)));   /* Ethernet Rx MA Descriptor */
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] __attribute__((at(0x20010080)));   /* Ethernet Tx DMA Descriptor */
uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __attribute__((at(0x2004C000)));   /* Ethernet Receive Buffer */
uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __attribute__((at(0x2004D7D0)));   /* Ethernet Transmit Buffer */


#elif defined (__ICCARM__) /*!< IAR Compiler */
  #pragma data_alignment=4 
#pragma location=0x20010000
__no_init ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB];                         /* Ethernet Rx MA Descriptor */

#pragma location=0x20010080
__no_init ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB];                         /* Ethernet Tx DMA Descriptor */

#pragma location=0x2004C000
__no_init uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE];                         /* Ethernet Receive Buffer */

#pragma location=0x2004D7D0
__no_init uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE];                         /* Ethernet Transmit Buffer */


#elif defined (__GNUC__)                                                         /*!< GNU Compiler */
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] __attribute__((section(".RxDecripSection")));  /* Ethernet Rx MA Descriptor */
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] __attribute__((section(".TxDescripSection"))); /* Ethernet Tx DMA Descriptor */
uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __attribute__((section(".RxarraySection")));   /* Ethernet Receive Buffer */
uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __attribute__((section(".TxarraySection")));   /* Ethernet Transmit Buffer */
#endif

/* Semaphore to signal incoming packets */
osSemaphoreId s_xSemaphore                               = NULL;

// ----------------------------------------------------------------------------
/* Global Ethernet handle */
ETH_HandleTypeDef heth;
// Mac address
uint8_t MY_MAC_ADDR[6]                                   = {0x00, 0x80, 0xE1, 0x00, 0x00, 0x00};

/* --------------------------------------------------------------------------
 * Name : HAL_ETH_RxCpltCallback()
 *        Ethernet Rx Transfer completed callback
 *
 * -------------------------------------------------------------------------- */
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
   osSemaphoreRelease(s_xSemaphore);
}

/*******************************************************************************
   LL Driver Interface ( LwIP stack --> ETH) 
*******************************************************************************/

/* --------------------------------------------------------------------------
 * Name : low_level_init()
 *        In this function, the hardware should be initialized.
 *        Called from ethernetif_init().
 *
 *        [param netif] the already initialized lwip network interface structure
 *                      for this ethernetif
 *
 * -------------------------------------------------------------------------- */
static void low_level_init(struct netif *netif)
{ 
   uint32_t regvalue                                     = 0;
   HAL_StatusTypeDef hal_eth_init_status;

   UNUSED(regvalue);

   debug_output_info("MAC ADDRESS : %02X-%02X-%02X-%02X-%02X-%02X \r\n", MY_MAC_ADDR[0], MY_MAC_ADDR[1], MY_MAC_ADDR[2], MY_MAC_ADDR[3], MY_MAC_ADDR[4], MY_MAC_ADDR[5]);

   /* Init ETH */
   heth.Instance                                         = ETH;
   heth.Init.MACAddr                                     = &MY_MAC_ADDR[0];
   heth.Init.AutoNegotiation                             = ETH_AUTONEGOTIATION_ENABLE;
   heth.Init.Speed                                       = ETH_SPEED_100M;
   heth.Init.DuplexMode                                  = ETH_MODE_FULLDUPLEX;
   heth.Init.MediaInterface                              = ETH_MEDIA_INTERFACE_RMII;
   heth.Init.RxMode                                      = ETH_RXINTERRUPT_MODE;
   heth.Init.ChecksumMode                                = ETH_CHECKSUM_BY_HARDWARE;
   heth.Init.PhyAddress                                  = 0x00U;

   hal_eth_init_status                                   = HAL_ETH_Init(&heth);

   if (hal_eth_init_status == HAL_OK)
   {
      /* Set netif link flag */  
      netif->flags                                       |= NETIF_FLAG_LINK_UP;
   }
   else
   {
      debug_output_error("HAL_ETH_Init() failed !!! : %d \r\n", hal_eth_init_status);
   }

   /* Initialize Tx Descriptors list: Chain Mode */
   hal_eth_init_status                                   = HAL_ETH_DMATxDescListInit(&heth, DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
   if (hal_eth_init_status != HAL_OK)
   {
      debug_output_error("HAL_ETH_DMATxDescListInit() failed : %d !!! \r\n", hal_eth_init_status);
   }
     
   /* Initialize Rx Descriptors list: Chain Mode  */
   hal_eth_init_status                                   = HAL_ETH_DMARxDescListInit(&heth, DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);
   if (hal_eth_init_status != HAL_OK)
   {
      debug_output_error("HAL_ETH_DMARxDescListInit() failed : %d !!! \r\n", hal_eth_init_status);
   }

#if LWIP_ARP || LWIP_ETHERNET
   /* set MAC hardware address length */
   netif->hwaddr_len                                     = ETH_HWADDR_LEN;

   /* set MAC hardware address */
   netif->hwaddr[0]                                      = heth.Init.MACAddr[0];
   netif->hwaddr[1]                                      = heth.Init.MACAddr[1];
   netif->hwaddr[2]                                      = heth.Init.MACAddr[2];
   netif->hwaddr[3]                                      = heth.Init.MACAddr[3];
   netif->hwaddr[4]                                      = heth.Init.MACAddr[4];
   netif->hwaddr[5]                                      = heth.Init.MACAddr[5];
  
  /* maximum transfer unit */
  netif->mtu                                             = 1500;
  
  /* Accept broadcast address and ARP traffic */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  #if LWIP_ARP
    netif->flags                                         |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
  #else 
    netif->flags                                         |= NETIF_FLAG_BROADCAST;
  #endif /* LWIP_ARP */
  
   /* create a binary semaphore used for informing ethernetif of frame reception */
   osSemaphoreDef(SEM);
   s_xSemaphore                                          = osSemaphoreCreate(osSemaphore(SEM), 1);

   /* Enable MAC and DMA transmission and reception */
   hal_eth_init_status                                   = HAL_ETH_Start(&heth); 
   if (hal_eth_init_status != HAL_OK)
   {
      debug_output_error("HAL_ETH_Start() failed : %d !!! \r\n", hal_eth_init_status);
   }

   /* create the task that handles the ETH_MAC */
   osThreadDef(EthIf, ethernetif_input, osPriorityRealtime, 0, INTERFACE_THREAD_STACK_SIZE);
   osThreadCreate(osThread(EthIf), netif);

#if 0 // from lwip application
   /* Read Register Configuration */
   HAL_ETH_ReadPHYRegister(&heth, PHY_ISFR, &regvalue);
   regvalue                                              |= (PHY_ISFR_INT4);

   /* Enable Interrupt on change of link status */ 
   HAL_ETH_WritePHYRegister(&heth, PHY_ISFR , regvalue);
  
   /* Read Register Configuration */
   HAL_ETH_ReadPHYRegister(&heth, PHY_ISFR , &regvalue);
#endif

#endif /* LWIP_ARP || LWIP_ETHERNET */
}

/* --------------------------------------------------------------------------
 * Name : low_level_output()
 *
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 *
 * -------------------------------------------------------------------------- */
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
   err_t errval;
   struct pbuf *q;
   uint8_t *buffer                                       = (uint8_t *)(heth.TxDesc->Buffer1Addr);
   __IO ETH_DMADescTypeDef *DmaTxDesc;
   uint32_t framelength                                  = 0;
   uint32_t bufferoffset                                 = 0;
   uint32_t byteslefttocopy                              = 0;
   uint32_t payloadoffset                                = 0;
   DmaTxDesc                                             = heth.TxDesc;
   bufferoffset                                          = 0;

   /* copy frame from pbufs to driver buffers */
   for (q = p; q != NULL; q = q->next)
   {
      /* Is this buffer available? If not, goto error */
      if ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET)
      {
         errval                                          = ERR_USE;
         /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to resume transmission */
         if ((heth.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET)
         {
            /* Clear TUS ETHERNET DMA flag */
            heth.Instance->DMASR                         = ETH_DMASR_TUS;

            /* Resume DMA transmission*/
            heth.Instance->DMATPDR                       = 0;
         }
         debug_output_error("low_level_output() %d failed \r\n", errval);
         return errval;
      }

      /* Get bytes in current lwIP buffer */
      byteslefttocopy                                    = q->len;
      payloadoffset                                      = 0;

      /* Check if the length of data to copy is bigger than Tx buffer size*/
      while ((byteslefttocopy + bufferoffset) > ETH_TX_BUF_SIZE)
      {
         /* Copy data to Tx buffer*/
         memcpy((uint8_t*)((uint8_t*) buffer + bufferoffset), (uint8_t*)((uint8_t*) q->payload + payloadoffset), (ETH_TX_BUF_SIZE - bufferoffset));

         /* Point to next descriptor */
         DmaTxDesc                                       = (ETH_DMADescTypeDef *)(DmaTxDesc->Buffer2NextDescAddr);

         /* Check if the buffer is available */
         if ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t) RESET)
         {
            errval                                       = ERR_USE;
            /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to resume transmission */
            if ((heth.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET)
            {
               /* Clear TUS ETHERNET DMA flag */
               heth.Instance->DMASR                      = ETH_DMASR_TUS;

               /* Resume DMA transmission*/
               heth.Instance->DMATPDR                    = 0;
            }
            debug_output_error("low_level_output() %d failed \r\n", errval);
            return errval;
         }

         buffer                                          = (uint8_t *)(DmaTxDesc->Buffer1Addr);

         byteslefttocopy                                 = byteslefttocopy - (ETH_TX_BUF_SIZE - bufferoffset);
         payloadoffset                                   = payloadoffset + (ETH_TX_BUF_SIZE - bufferoffset);
         framelength                                     = framelength + (ETH_TX_BUF_SIZE - bufferoffset);
         bufferoffset                                    = 0;
      }

      /* Copy the remaining bytes */
      memcpy((uint8_t*)((uint8_t*)buffer + bufferoffset), (uint8_t*)((uint8_t*)q->payload + payloadoffset), byteslefttocopy);
      bufferoffset                                       = bufferoffset + byteslefttocopy;
      framelength                                        = framelength + byteslefttocopy;
   }

   /* Clean and Invalidate data cache */
   SCB_CleanInvalidateDCache();
   /* Prepare transmit descriptors to give to DMA */ 
   HAL_ETH_TransmitFrame(&heth, framelength);
   errval = ERR_OK;
   return errval;
}

/* --------------------------------------------------------------------------
 * Name : low_level_input()
 *
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 *
 * -------------------------------------------------------------------------- */
static struct pbuf * low_level_input(struct netif *netif)
{
   struct pbuf *p                                        = NULL;
   struct pbuf *q                                        = NULL;
   uint16_t len                                          = 0;
   uint8_t *buffer;
   __IO ETH_DMADescTypeDef *dmarxdesc;
   uint32_t bufferoffset                                 = 0;
   uint32_t payloadoffset                                = 0;
   uint32_t byteslefttocopy                              = 0;
   uint32_t i                                            = 0;
   HAL_StatusTypeDef hal_status;
  
   /* get received frame */
   if ((hal_status = HAL_ETH_GetReceivedFrame_IT(&heth)) != HAL_OK)
   {
      return NULL;
   }

   /* Obtain the size of the packet and put it into the "len" variable. */
   len                                                   = heth.RxFrameInfos.length;
   buffer                                                = (uint8_t *) heth.RxFrameInfos.buffer;
  
   if (len > 0)
   {
      /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
      p                                                  = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
   }

   /* Clean and Invalidate data cache */
   SCB_CleanInvalidateDCache();

   if (p != NULL)
   {
      dmarxdesc                                          = heth.RxFrameInfos.FSRxDesc;
      bufferoffset                                       = 0;
      for (q = p; q != NULL; q = q->next)
      {
         byteslefttocopy                                 = q->len;
         payloadoffset                                   = 0;

         /* Check if the length of bytes to copy in current pbuf is bigger than Rx buffer size*/
         while ((byteslefttocopy + bufferoffset) > ETH_RX_BUF_SIZE)
         {
            /* Copy data to pbuf */
            memcpy((uint8_t*)((uint8_t*)q->payload + payloadoffset), (uint8_t*)((uint8_t*)buffer + bufferoffset), (ETH_RX_BUF_SIZE - bufferoffset));

            /* Point to next descriptor */
            dmarxdesc                                    = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
            buffer                                       = (uint8_t *)(dmarxdesc->Buffer1Addr);

            byteslefttocopy                              = byteslefttocopy - (ETH_RX_BUF_SIZE - bufferoffset);
            payloadoffset                                = payloadoffset + (ETH_RX_BUF_SIZE - bufferoffset);
            bufferoffset                                 = 0;
         }
         /* Copy remaining data in pbuf */
         memcpy((uint8_t*)((uint8_t*)q->payload + payloadoffset), (uint8_t*)((uint8_t*)buffer + bufferoffset), byteslefttocopy);
         bufferoffset                                    = bufferoffset + byteslefttocopy;
      }
   }  
  
   /* Release descriptors to DMA */
   /* Point to first descriptor */
   dmarxdesc                                             = heth.RxFrameInfos.FSRxDesc;
   /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
   for (i = 0; i < heth.RxFrameInfos.SegCount; i++)
   {  
      dmarxdesc->Status                                  |= ETH_DMARXDESC_OWN;
      dmarxdesc                                          = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
   }
    
    /* Clear Segment_Count */
    heth.RxFrameInfos.SegCount                           = 0;  
  
   /* When Rx Buffer unavailable flag is set: clear it and resume reception */
   if ((heth.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t) RESET)
   {
      /* Clear RBUS ETHERNET DMA flag */
      heth.Instance->DMASR                               = ETH_DMASR_RBUS;
      /* Resume DMA reception */
      heth.Instance->DMARPDR                             = 0;
   }
   return p;
}

/* --------------------------------------------------------------------------
 * Name : low_level_input()
 *
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 *
 * -------------------------------------------------------------------------- */
void ethernetif_input( void const * argument ) 
{
   struct pbuf *p;
   struct netif *netif                                   = (struct netif *) argument;

   for (;;)
   {
      if (osSemaphoreWait(s_xSemaphore, TIME_WAITING_FOR_INPUT) == osOK)
      {
         do
         {   
            p                                            = low_level_input(netif);
            if (p != NULL)
            {
               if (netif->input(p, netif) != ERR_OK)
               {
                  pbuf_free(p);
               }
            }
         } while(p != NULL);
      }
   }
}

#if !LWIP_ARP
/* --------------------------------------------------------------------------
 * Name : low_level_input()
 *
 * This function has to be completed by user in case of ARP OFF.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if ...
 *
 * -------------------------------------------------------------------------- */
static err_t low_level_output_arp_off(struct netif *netif, struct pbuf *q, ip_addr_t *ipaddr)
{  
   err_t errval;
   errval                                                = ERR_OK;
   return errval;  
}
#endif /* LWIP_ARP */ 

/* --------------------------------------------------------------------------
 * Name : low_level_input()
 *
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 *
 * -------------------------------------------------------------------------- */
err_t ethernetif_init(struct netif *netif)
{
   LWIP_ASSERT("netif != NULL", (netif != NULL));

   debug_output_info(" \r\n");

#if LWIP_NETIF_HOSTNAME
   /* Initialize interface hostname */
   netif->hostname                                       = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

   netif->name[0]                                        = IFNAME0;
   netif->name[1]                                        = IFNAME1;
   /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */

#if LWIP_IPV4
#if LWIP_ARP || LWIP_ETHERNET
#if LWIP_ARP
   netif->output                                         = etharp_output;
#else
   /* The user should write ist own code in low_level_output_arp_off function */
   netif->output                                         = low_level_output_arp_off;
#endif /* LWIP_ARP */
#endif /* LWIP_ARP || LWIP_ETHERNET */
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
   netif->output_ip6                                     = ethip6_output;
#endif /* LWIP_IPV6 */

   netif->linkoutput                                     = low_level_output;

   /* initialize the hardware */
   low_level_init(netif);

   return ERR_OK;
}

/* --------------------------------------------------------------------------
 * Name : sys_jiffies()
 *
 * @brief  Returns the current time in milliseconds
 *         when LWIP_TIMERS == 1 and NO_SYS == 1
 * @param  None
 * @retval Time
 *
 * -------------------------------------------------------------------------- */
u32_t sys_jiffies(void)
{
   return HAL_GetTick();
}

/* --------------------------------------------------------------------------
 * Name : sys_now()
 *
 * @brief  Returns the current time in milliseconds
 *         when LWIP_TIMERS == 1 and NO_SYS == 1
 * @param  None
 * @retval Time
 *
 * -------------------------------------------------------------------------- */
u32_t sys_now(void)
{
   return HAL_GetTick();
}

#if LWIP_NETIF_LINK_CALLBACK
/* --------------------------------------------------------------------------
 * Name : ethernetif_update_config()
 *
 * @brief  Link callback function, this function is called on change of link status
 *         to update low level driver configuration.
 * @param  netif: The network interface
 * @retval None
 *
 * -------------------------------------------------------------------------- */
void ethernetif_update_config(struct netif *netif)
{
   __IO uint32_t tickstart                               = 0;
   uint32_t regvalue                                     = 0;

   if (netif_is_link_up(netif))
   {
      debug_output_info("link-up event !!! \r\n");
      /* Restart the auto-negotiation */
      if (heth.Init.AutoNegotiation != ETH_AUTONEGOTIATION_DISABLE)
      {
         /* Enable Auto-Negotiation */
         HAL_ETH_WritePHYRegister(&heth, PHY_BCR, PHY_AUTONEGOTIATION);

         /* Get tick */
         tickstart                                       = HAL_GetTick();

         /* Wait until the auto-negotiation will be completed */
         do
         {
            HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &regvalue);

            /* Check for the Timeout ( 1s ) */
            if ((HAL_GetTick() - tickstart ) > 1000)
            {     
               /* In case of timeout */ 
               goto error;
            }   
         } while (((regvalue & PHY_AUTONEGO_COMPLETE) != PHY_AUTONEGO_COMPLETE));

         /* Read the result of the auto-negotiation */
         HAL_ETH_ReadPHYRegister(&heth, PHY_SR, &regvalue);

         /* Configure the MAC with the Duplex Mode fixed by the auto-negotiation process */
         if ((regvalue & PHY_DUPLEX_STATUS) != (uint32_t) RESET)
         {
            /* Set Ethernet duplex mode to Full-duplex following the auto-negotiation */
            heth.Init.DuplexMode                         = ETH_MODE_FULLDUPLEX;  
            debug_output_info("ETHERNET : Full Duplex ... \r\n");
         }
         else
         {
            /* Set Ethernet duplex mode to Half-duplex following the auto-negotiation */
            heth.Init.DuplexMode                         = ETH_MODE_HALFDUPLEX;           
            debug_output_info("ETHERNET : Half Duplex ... \r\n");
         }
         /* Configure the MAC with the speed fixed by the auto-negotiation process */
         if (regvalue & PHY_SPEED_STATUS)
         {  
            /* Set Ethernet speed to 10M following the auto-negotiation */
            heth.Init.Speed                              = ETH_SPEED_10M; 
            debug_output_info("ETHERNET : 10M ... \r\n");
         }
         else
         {   
            /* Set Ethernet speed to 100M following the auto-negotiation */ 
            heth.Init.Speed                              = ETH_SPEED_100M;
            debug_output_info("ETHERNET : 100M ... \r\n");
         }
      }
      else /* AutoNegotiation Disable */
      {
         error :
         /* Check parameters */
         assert_param(IS_ETH_SPEED(heth.Init.Speed));
         assert_param(IS_ETH_DUPLEX_MODE(heth.Init.DuplexMode));

         /* Set MAC Speed and Duplex Mode to PHY */
         HAL_ETH_WritePHYRegister(&heth, PHY_BCR, ((uint16_t)(heth.Init.DuplexMode >> 3) | (uint16_t)(heth.Init.Speed >> 1))); 
         debug_output_info("ETHERNET : error parameter ... \r\n");
      }

      /* ETHERNET MAC Re-Configuration */
      HAL_ETH_ConfigMAC(&heth, (ETH_MACInitTypeDef *) NULL);

      /* Restart MAC interface */
      HAL_ETH_Start(&heth);   
   }
   else
   {
      debug_output_info("link-down event !!! \r\n");
      /* Stop MAC interface */
      HAL_ETH_Stop(&heth);
   }

   ethernetif_notify_conn_changed(netif);
}

/* --------------------------------------------------------------------------
 * Name : ethernetif_notify_conn_changed()
 *
 * @brief  This function notify user about link status changement.
 * @param  netif: the network interface
 * @retval None
 *
 * -------------------------------------------------------------------------- */
__weak void ethernetif_notify_conn_changed(struct netif *netif)
{
   /* NOTE : This is function could be implemented in user file 
             when the callback is needed,
   */
}
#endif /* LWIP_NETIF_LINK_CALLBACK */





