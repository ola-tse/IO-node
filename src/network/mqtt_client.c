#include "mqtt_client.h"
#include <zephyr/net/socket.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/entropy.h>
#include <zephyr/random/random.h>
#include <zephyr/app_memory/app_memdomain.h>


LOG_MODULE_REGISTER(mqtt_client, LOG_LEVEL_DBG);

static uint8_t rx_buffer[512];
static uint8_t tx_buffer[512];
struct mqtt_client client;
static struct sockaddr_storage broker;
static bool mqtt_connected = false;


static void mqtt_evt_handler(struct mqtt_client *const client, const struct mqtt_evt *evt)
{
    switch (evt->type) {
    case MQTT_EVT_CONNACK:
        if (evt->result != 0) {
            LOG_ERR("MQTT connect failed %d", evt->result);
            break;
        }
        mqtt_connected = true;
        LOG_INF("MQTT client connected!");
        break;

    case MQTT_EVT_DISCONNECT:
        LOG_INF("MQTT client disconnected %d", evt->result);
        mqtt_connected = false;
        break;

    case MQTT_EVT_PINGRESP:
        LOG_INF("MQTT ping response received");
        break;

    default:
        break;
    }
}

void mqtt_init(void)
{
    struct sockaddr_in *broker4 = (struct sockaddr_in *)&broker;
    broker4->sin_family = AF_INET;
    broker4->sin_port = htons(1883); // MQTT broker port
    inet_pton(AF_INET, "192.168.80.115", &broker4->sin_addr); // MQTT broker IP

    mqtt_client_init(&client);
    client.broker = &broker;
    client.evt_cb = mqtt_evt_handler;
    client.client_id.utf8 = (uint8_t *)"zephyr_client";
    client.client_id.size = strlen("zephyr_client");
    client.protocol_version = MQTT_VERSION_3_1_0;

    client.rx_buf = rx_buffer;
    client.rx_buf_size = sizeof(rx_buffer);
    client.tx_buf = tx_buffer;
    client.tx_buf_size = sizeof(tx_buffer);
}

int mqtt_connect_broker(void)
{
    int rc = mqtt_connect(&client);
    if (rc != 0) {
        LOG_ERR("mqtt_connect failed: %d", rc);
    }
    return rc;
}

int mqtt_publish_message(const char *topic, const char *payload)
{
    struct mqtt_publish_param param;
    param.message.topic.qos = MQTT_QOS_0_AT_MOST_ONCE;
    param.message.topic.topic.utf8 = (uint8_t *)topic;
    param.message.topic.topic.size = strlen(topic);
    param.message.payload.data = (uint8_t *)payload;
    param.message.payload.len = strlen(payload);
    param.message_id = sys_rand32_get();
    param.dup_flag = 0U;
    param.retain_flag = 0U;

    LOG_INF("MQTT Publish Parameters:");
    LOG_INF("Topic: %s", param.message.topic.topic.utf8);
    LOG_INF("Payload Length: %d", param.message.payload.len);
    LOG_INF("Message ID: %d", param.message_id);

    int rc = mqtt_publish(&client, &param);
    if (rc != 0) {
        LOG_ERR("mqtt_publish failed: %d", rc);
    }

    return rc;
}
