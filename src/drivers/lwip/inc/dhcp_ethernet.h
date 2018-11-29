/*
 * File : dhcp_ethernet.h
 *
 * Copyright (c) duvallee
 *
 */

#ifndef __DHCP_ETHERNET_H__
#define __DHCP_ETHERNET_H__

#define DHCP_OFF                                         (uint8_t) 0
#define DHCP_START                                       (uint8_t) 1
#define DHCP_WAIT_ADDRESS                                (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED                            (uint8_t) 3
#define DHCP_TIMEOUT                                     (uint8_t) 4
#define DHCP_LINK_DOWN                                   (uint8_t) 5

int is_assigned_dhcp_address(void);
void DHCP_Task(void const * argument);

#endif   // __DHCP_ETHERNET_H__

