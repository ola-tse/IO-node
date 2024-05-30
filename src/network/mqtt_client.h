#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <zephyr/net/mqtt.h>

#define APP_CONNECT_TIMEOUT_MS	2000

extern bool connected2broker;

void my_mqtt_client_init(void);
int mqtt_connect_to_broker(void);
int mqtt_publish_message(const char *topic, const char *payload);
int mqtt_subscribe_topic(const char *topic);
void mqtt_unsubscribe_topic(const char *topic);
void mqtt_disconnect_from_broker(void);
int process_mqtt_and_sleep(int timeout);

#endif // MQTT_CLIENT_H
