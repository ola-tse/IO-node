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

void dhcp_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface) {
    if (mgmt_event == NET_EVENT_IPV4_ADDR_ADD) {
        struct net_if_addr *ifaddr;
        char buf[NET_IPV4_ADDR_LEN];

        ifaddr = net_if_ipv4_addr_lookup(&iface->config.ip.ipv4->unicast[0].address.in_addr, &iface);
        if (ifaddr) {
            net_addr_ntop(AF_INET, &ifaddr->address.in_addr, buf, sizeof(buf));
            int a = 0;
            LOG_INF("obtained IP address: %s", buf);
            printk("obtained IP address: %s", buf);
        } else {
            LOG_ERR("Failed to get DHCP address");
            printk("Failed to get DHCP address");
        }
    }
}