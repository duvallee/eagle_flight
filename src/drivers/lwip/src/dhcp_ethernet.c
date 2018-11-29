
/*
 * File : dhcp_ethernet.c
 *
 * Copyright (c) duvallee
 *
*/
#include "main.h"
#include "lwip.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"

#include "cmsis_os.h"
#include "dhcp_ethernet.h"

#if (LWIP_DHCP == 1)
#define MAX_DHCP_TRIES                                   4
__IO uint8_t DHCP_state                                  = DHCP_OFF;


/*Static IP ADDRESS*/
#define IP_ADDR0                                         192
#define IP_ADDR1                                         168
#define IP_ADDR2                                         1
#define IP_ADDR3                                         56
   
/*NETMASK*/
#define NETMASK_ADDR0                                    255
#define NETMASK_ADDR1                                    255
#define NETMASK_ADDR2                                    255
#define NETMASK_ADDR3                                    0

/*Gateway Address*/
#define GW_ADDR0                                         192
#define GW_ADDR1                                         168
#define GW_ADDR2                                         1
#define GW_ADDR3                                         1


/* --------------------------------------------------------------------------
 * Name : is_assigned_dhcp_address()
 *
 *
 * -------------------------------------------------------------------------- */
int is_assigned_dhcp_address(void)
{
   if (DHCP_state == DHCP_ADDRESS_ASSIGNED)
   {
      return 1;
   }
   return 0;
}

/* --------------------------------------------------------------------------
 * Name : DHCP_Task()
 *
 *
 * -------------------------------------------------------------------------- */
void DHCP_Task(void const * argument)
{
   struct netif *netif                                   = (struct netif *) argument;
   ip_addr_t ipaddr;
   ip_addr_t netmask;
   ip_addr_t gw;
   struct dhcp *dhcp;
   uint8_t iptxt[20];

   UNUSED(netif);
   UNUSED(ipaddr);
   UNUSED(netmask);
   UNUSED(gw);
   UNUSED(dhcp);
   UNUSED(iptxt);

   debug_output_info(" \r\n");
   osDelay(250);

   if (netif_is_up(netif))
   {
      DHCP_state                                         = DHCP_START;
   }

   for (;;)
   {
      switch (DHCP_state)
      {
         case DHCP_START :
            {
               ip_addr_set_zero_ip4(&netif->ip_addr);
               ip_addr_set_zero_ip4(&netif->netmask);
               ip_addr_set_zero_ip4(&netif->gw);       
               dhcp_start(netif);
               DHCP_state                                = DHCP_WAIT_ADDRESS;
               debug_output_info ("  State: Looking for DHCP server ...\n");
            }
            break;

         case DHCP_WAIT_ADDRESS :
            {
               if (dhcp_supplied_address(netif)) 
               {
                  DHCP_state                             = DHCP_ADDRESS_ASSIGNED;	

                  sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));   
                  debug_output_info ("IP address assigned by a DHCP server: %s\n", iptxt);
               }
               else
               {
                  dhcp                                   = (struct dhcp *) netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);

                  /* DHCP timeout */
                  if (dhcp->tries > MAX_DHCP_TRIES)
                  {
                     DHCP_state                          = DHCP_TIMEOUT;

                     /* Stop DHCP */
                     dhcp_stop(netif);

                     /* Static address used */
                     IP_ADDR4(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
                     IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
                     IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
                     netif_set_addr(netif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));

                     sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));
                     debug_output_info ("DHCP Timeout !! \n");
                     debug_output_info ("Static IP address: %s\n", iptxt);  
                  }
               }
            }
            break;

         case DHCP_LINK_DOWN :
         {
            /* Stop DHCP */
            dhcp_stop(netif);
            DHCP_state                                   = DHCP_OFF; 
         }
            break;

         default :
            break;
      }

      /* wait 250 ms */
      osDelay(250);
   }

#if 0
   /* Start DHCP negotiation for a network interface (IPv4) */
   if ((result = dhcp_start(&gnetif)) != ERR_OK)
   {
      debug_output_error("netif_add() failed : %d !!!", result);
      return;
   }
#endif

}
#endif

