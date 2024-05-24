#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/net/net_if.h>

#include <zephyr/net/net_mgmt.h>
#include <zephyr/logging/log.h>


#include "sensors/ds18b20.h"
#include "network/mqtt_client.h"
#include "network/general.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define LED0_NODE DT_ALIAS(led0)
#define LED2_NODE DT_ALIAS(led2)

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);


static void dhcp_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface) {
    if (mgmt_event == NET_EVENT_IPV4_ADDR_ADD) {
        struct net_if_addr *ifaddr;
        char buf[NET_IPV4_ADDR_LEN];

        ifaddr = net_if_ipv4_addr_lookup(&iface->config.ip.ipv4->unicast[0].address.in_addr, &iface);
        if (ifaddr) {
            net_addr_ntop(AF_INET, &ifaddr->address.in_addr, buf, sizeof(buf));
            LOG_INF("IP address: %s", buf);
        } else {
            LOG_ERR("Failed to get DHCP address");
        }
    }
}

int main(void) {
    LOG_INF("Hello from main!");
    store_mac_address();

    if (!device_is_ready(led0.port) || !device_is_ready(led2.port)) {
        LOG_ERR("LED devices not ready");
        return -1;
    }
    gpio_pin_configure_dt(&led0, GPIO_OUTPUT_LOW);
    gpio_pin_configure_dt(&led2, GPIO_OUTPUT_LOW);

    struct device *dev = get_ds18b20_device();
    if (!dev) {
        LOG_ERR("Failed to get DS18B20 device");
        return -1;
    }

    struct net_if *iface = net_if_get_default();
    struct net_mgmt_event_callback cb;

    net_mgmt_init_event_callback(&cb, dhcp_handler, NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&cb);

    net_dhcpv4_start(iface);

    while (1) {
        struct sensor_value temp;

        int sample_ok = sensor_sample_fetch(dev);
        int channel_ok = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);

        printk("Stored MAC address: %s\n", stored_mac_address);

        if (sample_ok == 0 && channel_ok == 0) {
            printk("Temp: %d.%06d\n", temp.val1, temp.val2);
        } else {
            LOG_ERR("Failed to fetch temperature data");
        }

        gpio_pin_toggle_dt(temp.val1 != 0 ? &led0 : &led2);
        k_sleep(K_MSEC(1000));
    }

    return 0;
}
