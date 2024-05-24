#ifndef NET_GENERAL_H
#define NET_GENERAL_H

#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>

extern char stored_mac_address[18];

void store_mac_address(void);

#endif // NET_GENERAL_H