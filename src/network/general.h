#ifndef NET_GENERAL_H
#define NET_GENERAL_H

#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>

#define STM32_IPADDR "192.168.80.132"

extern char stored_mac_address[18];

void store_mac_address(void);
void dhcp_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface);

#endif // NET_GENERAL_H