#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <string.h> // For memcpy
#include <zephyr/logging/log.h> // Include the logging module
#include "general.h"

LOG_MODULE_REGISTER(network_general, LOG_LEVEL_INF);

// Define the global variable to store the MAC address
char stored_mac_address[18] = {0};

void store_mac_address(void) {
    struct net_if *iface = net_if_get_default();
    struct net_linkaddr *link_addr = net_if_get_link_addr(iface);

    snprintf(stored_mac_address, sizeof(stored_mac_address), "%02x:%02x:%02x:%02x:%02x:%02x",
             link_addr->addr[0], link_addr->addr[1], link_addr->addr[2],
             link_addr->addr[3], link_addr->addr[4], link_addr->addr[5]);

    LOG_INF("Stored MAC address: %s", stored_mac_address);
}