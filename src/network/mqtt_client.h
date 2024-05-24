#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <zephyr/kernel.h>
#include <zephyr/net/mqtt.h>
#include <zephyr/net/socket.h>
#include <stdio.h>
#include <string.h>

#define MQTT_BROKER_ADDR "192.168.80.115"
#define MQTT_BROKER_PORT 1883
#define MQTT_CLIENT_ID "controller"
#define MQTT_USERNAME "your_username"
#define MQTT_PASSWORD "your_password"
#define RX_BUFFER_SIZE 128
#define TX_BUFFER_SIZE 128

extern bool mqtt_connected;
extern uint8_t rx_buffer[RX_BUFFER_SIZE];
extern uint8_t tx_buffer[TX_BUFFER_SIZE];

extern struct mqtt_client client;
extern struct sockaddr_storage broker;
extern struct mqtt_utf8 client_id;
extern struct mqtt_utf8 user_name;
extern struct mqtt_utf8 password;

void mqtt_init(void);
void mqtt_event_handler(struct mqtt_client *client, const struct mqtt_evt *evt);
int publish(struct mqtt_client *client, char *topic, char *payload);

#endif // MQTT_CLIENT_H
