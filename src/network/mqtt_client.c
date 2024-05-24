#include "mqtt_client.h"

uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t tx_buffer[TX_BUFFER_SIZE];

struct mqtt_client client;
struct sockaddr_storage broker;

struct mqtt_utf8 client_id = {
    .utf8 = MQTT_CLIENT_ID,
    .size = strlen(MQTT_CLIENT_ID)
};

struct mqtt_utf8 user_name = {
    .utf8 = MQTT_USERNAME,
    .size = strlen(MQTT_USERNAME)
};

struct mqtt_utf8 password = {
    .utf8 = MQTT_PASSWORD,
    .size = strlen(MQTT_PASSWORD)
};

bool mqtt_connected = false;

void mqtt_event_handler(struct mqtt_client *client, const struct mqtt_evt *evt) {
    switch (evt->type) {
        case MQTT_EVT_CONNACK:
            if (evt->result == 0) {
                printk("MQTT connected!\n");
                mqtt_connected = true;
            } else {
                printk("MQTT connect failed: %d\n", evt->result);
            }
            break;
        case MQTT_EVT_DISCONNECT:
            printk("MQTT disconnected: %d\n", evt->result);
            mqtt_connected = false;
            break;
        default:
            break;
    }
}

void mqtt_init(void) {
    struct sockaddr_in *broker4 = (struct sockaddr_in *)&broker;

    broker4->sin_family = AF_INET;
    broker4->sin_port = htons(MQTT_BROKER_PORT);
    inet_pton(AF_INET, MQTT_BROKER_ADDR, &broker4->sin_addr);

    mqtt_client_init(&client);

    client.broker = &broker;
    client.evt_cb = mqtt_event_handler;
    client.client_id = client_id;
    //client.user_name = &user_name;
    //client.password = &password;
    client.protocol_version = MQTT_VERSION_3_1_1;

    client.rx_buf = rx_buffer;
    client.rx_buf_size = sizeof(rx_buffer);
    client.tx_buf = tx_buffer;
    client.tx_buf_size = sizeof(tx_buffer);
}

int publish(struct mqtt_client *client, char *topic, char *payload) {
    struct mqtt_publish_param param;

    param.message.topic.qos = MQTT_QOS_1_AT_LEAST_ONCE;
    param.message.topic.topic.utf8 = topic;
    param.message.topic.topic.size = strlen(topic);
    param.message.payload.data = payload;
    param.message.payload.len = strlen(payload);
    param.message_id = k_uptime_get_32();
    param.dup_flag = 0;
    param.retain_flag = 0;

    return mqtt_publish(client, &param);
}
