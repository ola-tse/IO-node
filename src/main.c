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
//#include "network/mqtt_client.h"
#include "network/mqtt_handlers.h"
#include "network/general.h"
#include "DO/DO.h"

// tmp START
#include <zephyr/net/mqtt.h>
#define MQTT_BROKER_ADDR "192.168.80.115" 
#define MQTT_BROKER_PORT 1883
#define MAX_RETRY_ATTEMPTS 5

//tmp END


LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define LED0_NODE DT_ALIAS(led0)
#define LED2_NODE DT_ALIAS(led2)

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);

static void wait_for_network_ready(void)
{
    struct net_if *iface = net_if_get_default();
    while (!net_if_is_up(iface)) {
        k_sleep(K_MSEC(100));
    }

    struct net_if_config *cfg = net_if_get_config(iface);
    while (!cfg->ip.ipv4->unicast[0].address.family) {
        k_sleep(K_MSEC(100));
    }
}
/*
static int connect_to_mqtt_broker(void)
{
    int rc, attempts = 0;

    while (attempts < MAX_RETRY_ATTEMPTS) {
        rc = mqtt_connect_broker();
        if (rc == 0) {
            return 0;
        }
        LOG_ERR("Failed to connect to MQTT broker: %d, attempt %d", rc, attempts + 1);
        attempts++;
        k_sleep(K_SECONDS(5));
    }

    return -1;
}
*/ 
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

    struct net_if *iface = net_if_get_default();
    struct net_mgmt_event_callback cb;

    net_mgmt_init_event_callback(&cb, dhcp_handler, NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&cb);

    net_dhcpv4_start(iface);
    
    wait_for_network_ready();

    LOG_INF("Network is ready");

    
    if (!iface) {
        LOG_ERR("No default network interface found.");
        return;
    }

    struct net_if_config *cfg = net_if_get_config(iface);
    if (!cfg) {
        LOG_ERR("Failed to get network interface configuration.");
        return;
    }

    char buf[NET_IPV4_ADDR_LEN];
    LOG_INF("Interface is %s", net_if_is_up(iface) ? "up" : "down");
    LOG_INF("IPv4 Address: %s", net_addr_ntop(AF_INET, &cfg->ip.ipv4->unicast[0].address.in_addr, buf, sizeof(buf)));

    // Initialize MQTT
    // mqtt_init();
    // int rc = 0;
    // Connect to MQTT broker
    // rc = mqtt_connect_broker();
    // if (rc != 0) {
    //     LOG_ERR("Failed to connect to MQTT broker: %d", rc);
    // } else {
    //     LOG_INF("Did not fail to connect to MQTT broker: %d", rc);
    // }

    // if (connect_to_mqtt_broker() != 0) {
    //     LOG_ERR("Could not establish connection to MQTT broker");
    //     return -1;
    // }

    // Wait for connection to be established
    k_sleep(K_MSEC(2000));

    // Publish a test message
    // rc = mqtt_publish_message("test/topic", "Hello, Zephyr!");
    // if (rc != 0) {
    //     LOG_ERR("Failed to publish message: %d", rc);
    // } else {
    //     LOG_INF("Message published successfully");
    // }

    while (1) {
        start_app();
        struct sensor_value temp;
        do_1_target++;
        LOG_INF("do target: %d", do_1_target);
        int sample_ok = sensor_sample_fetch(dev);
        int channel_ok = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);

        if (sample_ok == 0 && channel_ok == 0) {
            printk("Temperature is: %d.%06d\n", temp.val1, temp.val2);
        } else {
            LOG_ERR("Failed to fetch temperature data");
        }

        gpio_pin_toggle_dt(temp.val1 != 0 ? &led0 : &led2);
        k_sleep(K_MSEC(4000));

    //  rc = mqtt_publish_message("test/topic", "Hello, Zephyr!");
    //     if (rc != 0) {
    //         LOG_ERR("Failed to publish message: %d", rc);
    //     } else {
    //         LOG_INF("Message published successfully");
    // }
        k_yield();
    }

    return 0;
}


