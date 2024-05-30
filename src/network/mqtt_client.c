#include "mqtt_client.h"
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/random/random.h>
#include <string.h>

LOG_MODULE_REGISTER(mqtt_client, LOG_LEVEL_DBG);

#define SERVER_ADDR "192.168.80.115"
#define SERVER_PORT 1883
#define MQTT_CLIENTID "zephyr_client"
#define APP_MQTT_BUFFER_SIZE 128

static uint8_t rx_buffer[APP_MQTT_BUFFER_SIZE];
static uint8_t tx_buffer[APP_MQTT_BUFFER_SIZE];

static struct mqtt_client client_ctx;
static struct sockaddr_storage broker;
static struct zsock_pollfd fds[1];
static int nfds;
extern bool connected2broker = false;

static void prepare_fds(struct mqtt_client *client)
{
    fds[0].fd = client->transport.tcp.sock;
    fds[0].events = ZSOCK_POLLIN;
    nfds = 1;
}

static void clear_fds(void)
{
    nfds = 0;
}

static int wait(int timeout)
{
    int ret = 0;

    if (nfds > 0) {
        ret = zsock_poll(fds, nfds, timeout);
        if (ret < 0) {
            LOG_ERR("poll error: %d", errno);
        }
    }

    return ret;
}

void mqtt_evt_handler(struct mqtt_client *const client,
                      const struct mqtt_evt *evt) {
    int err;

    switch (evt->type) {
        case MQTT_EVT_CONNACK:
            if (evt->result != 0) {
                LOG_ERR("MQTT connect failed %d", evt->result);
                break;
            }
            LOG_INF("MQTT client connected!");
            connected2broker = true;
            break;

        case MQTT_EVT_DISCONNECT:
            LOG_INF("MQTT client disconnected %d", evt->result);
            break;

        case MQTT_EVT_PUBLISH:
            LOG_INF("MQTT PUBLISH event: id=%d len=%d",
                    evt->param.publish.message_id, evt->param.publish.message.payload.len);

            // Process the payload here
            char payload[128];
            memcpy(payload, evt->param.publish.message.payload.data, evt->param.publish.message.payload.len);
            payload[evt->param.publish.message.payload.len] = '\0';
            LOG_INF("Received: %s", payload);

            err = mqtt_publish_qos1_ack(client, evt->param.publish.message_id);
            if (err) {
                LOG_ERR("Failed to send MQTT PUBACK: %d", err);
            }
            break;

        case MQTT_EVT_PUBACK:
            if (evt->result != 0) {
                LOG_ERR("MQTT PUBACK error %d", evt->result);
                break;
            }
            LOG_INF("PUBACK packet id: %u", evt->param.puback.message_id);
            break;

        case MQTT_EVT_SUBACK:
            LOG_INF("SUBACK packet id: %u", evt->param.suback.message_id);
            break;

        case MQTT_EVT_UNSUBACK:
            LOG_INF("UNSUBACK packet id: %u", evt->param.unsuback.message_id);
            break;

        case MQTT_EVT_PINGRESP:
            LOG_INF("PINGRESP packet");
            break;

        default:
            LOG_INF("Unhandled MQTT event type: %d", evt->type);
            break;
    }
}
static void broker_init(void)
{
    struct sockaddr_in *broker4 = (struct sockaddr_in *)&broker;

    broker4->sin_family = AF_INET;
    broker4->sin_port = htons(SERVER_PORT);
    zsock_inet_pton(AF_INET, SERVER_ADDR, &broker4->sin_addr);
}

static void client_init(struct mqtt_client *client)
{
    mqtt_client_init(client);

    broker_init();

    /* MQTT client configuration */
    client->broker = &broker;
    client->evt_cb = mqtt_evt_handler;
    client->client_id.utf8 = (uint8_t *)MQTT_CLIENTID;
    client->client_id.size = strlen(MQTT_CLIENTID);
    client->password = NULL;
    client->user_name = NULL;
    client->protocol_version = MQTT_VERSION_3_1_0;

    /* MQTT buffers configuration */
    client->rx_buf = rx_buffer;
    client->rx_buf_size = sizeof(rx_buffer);
    client->tx_buf = tx_buffer;
    client->tx_buf_size = sizeof(tx_buffer);

    /* MQTT transport configuration */
    client->transport.type = MQTT_TRANSPORT_NON_SECURE;
}

void my_mqtt_client_init(void)
{
    client_init(&client_ctx);
}

int mqtt_connect_to_broker(void)
{
    int rc = mqtt_connect(&client_ctx);
    if (rc != 0) {
        LOG_ERR("mqtt_connect failed: %d", rc);
        return rc;
    }

    prepare_fds(&client_ctx);

    while (!connected2broker) {
        if (wait(APP_CONNECT_TIMEOUT_MS)) {
            mqtt_input(&client_ctx);
        }
    }

    return connected2broker ? 0 : -1;
}

int mqtt_publish_message(const char *topic, const char *payload)
{
    struct mqtt_publish_param param;

    param.message.topic.qos = MQTT_QOS_1_AT_LEAST_ONCE;
    param.message.topic.topic.utf8 = (uint8_t *)topic;
    param.message.topic.topic.size = strlen(topic);
    param.message.payload.data = (uint8_t *)payload;
    param.message.payload.len = strlen(payload);
    param.message_id = sys_rand32_get();
    param.dup_flag = 0U;
    param.retain_flag = 0U;

    return mqtt_publish(&client_ctx, &param);
}

int mqtt_subscribe_topic(const char *topic)
{
    struct mqtt_topic subscribe_topic;
    struct mqtt_subscription_list subscription_list;

    subscribe_topic.topic.utf8 = (uint8_t *)topic;
    subscribe_topic.topic.size = strlen(topic);
    subscribe_topic.qos = MQTT_QOS_1_AT_LEAST_ONCE;

    subscription_list.list = &subscribe_topic;
    subscription_list.list_count = 1;
    subscription_list.message_id = sys_rand32_get();

    return mqtt_subscribe(&client_ctx, &subscription_list);
}

void mqtt_unsubscribe_topic(const char *topic)
{
    struct mqtt_topic unsubscribe_topic;
    struct mqtt_subscription_list subscription_list;

    unsubscribe_topic.topic.utf8 = (uint8_t *)topic;
    unsubscribe_topic.topic.size = strlen(topic);
    unsubscribe_topic.qos = MQTT_QOS_1_AT_LEAST_ONCE;

    subscription_list.list = &unsubscribe_topic;
    subscription_list.list_count = 1;
    subscription_list.message_id = sys_rand32_get();

    mqtt_unsubscribe(&client_ctx, &subscription_list);
}

void mqtt_disconnect_from_broker(void)
{
    mqtt_disconnect(&client_ctx);
    connected2broker = false;
}

int process_mqtt_and_sleep(int timeout)
{
    int64_t remaining = timeout;
    int64_t start_time = k_uptime_get();
    int rc;

    while (remaining > 0 && connected2broker) {
        if (wait(remaining)) {
            rc = mqtt_input(&client_ctx);
            if (rc != 0) {
                LOG_ERR("mqtt_input failed: %d", rc);
                return rc;
            }
        }

        rc = mqtt_live(&client_ctx);
        if (rc != 0 && rc != -EAGAIN) {
            LOG_ERR("mqtt_live failed: %d", rc);
            return rc;
        } else if (rc == 0) {
            rc = mqtt_input(&client_ctx);
            if (rc != 0) {
                LOG_ERR("mqtt_input failed: %d", rc);
                return rc;
            }
        }

        remaining = timeout + start_time - k_uptime_get();
    }

    return 0;
}
