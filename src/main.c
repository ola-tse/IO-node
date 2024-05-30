#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/net_ip.h>
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

static void wait_for_network_ready(void)
{
    struct net_if *iface = net_if_get_default();
    char addr_str[NET_IPV4_ADDR_LEN];
    
    while (!net_if_is_up(iface)) {
        k_sleep(K_MSEC(100));
    }

    struct net_if_config *cfg = net_if_get_config(iface);
    while (!cfg->ip.ipv4->unicast[0].address.family) {
        k_sleep(K_MSEC(100));
    }

    while (true) {
        if (cfg->ip.ipv4 && cfg->ip.ipv4->unicast[0].address.family == AF_INET) {
            net_addr_ntop(AF_INET, &cfg->ip.ipv4->unicast[0].address.in_addr, addr_str, sizeof(addr_str));
            if (strcmp(addr_str, STM32_IPADDR) == 0) {
                LOG_INF("Obtained target IP address: %s", addr_str);
                break;
            }
        }
        k_sleep(K_MSEC(100));
    }
}

int main(void) {
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

    net_dhcpv4_start(net_if_get_default());
    wait_for_network_ready();

    LOG_INF("Network is ready");

    my_mqtt_client_init();
    if (mqtt_connect_to_broker() != 0) {
        LOG_ERR("Could not establish connection to MQTT broker");
        return -1;
    }

    while (1) {
        char payload[32];
        
        int rc;
        struct sensor_value temp;
        int sample_ok = sensor_sample_fetch(dev);
        int channel_ok = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
        
    
        snprintf(payload, sizeof(payload), "%d.%06d", temp.val1, abs(temp.val2));
        printk("Temperature is: %d.%06d\n", temp.val1, abs(temp.val2));
        
        if (sample_ok == 0 && channel_ok == 0) {
            printk("Temperature is: %d.%06d\n", temp.val1, temp.val2);
        } else {
            LOG_ERR("Failed to fetch temperature data");
        }

        gpio_pin_toggle_dt(temp.val1 != 0 ? &led0 : &led2);
        k_sleep(K_MSEC(1000));

        if (connected2broker) {
            rc = process_mqtt_and_sleep(1000);
            if (rc != 0) {
                LOG_ERR("Error in process_mqtt_and_sleep: %d", rc);
                break;
            }
            snprintf(payload, sizeof(payload), "%d.%06d", temp.val1, abs(temp.val2));
            printk("Temperature is: %d.%06d\n", temp.val1, abs(temp.val2));
            rc = mqtt_publish_message("sensors/temperature", payload);
            if (rc != 0) {
                LOG_ERR("Failed to publish message: %d", rc);
            }

            k_sleep(K_SECONDS(1));
        }
    }

    mqtt_disconnect_from_broker();
    return 0;
}