#include "variables.h"
#include "../network/mqtt_client.h"
#include <stdio.h>

// DO Variables
bool heater_1_request_on = false;
bool heater_2_request_on = false;
bool heater_3_request_on = false;
bool lights_1_request_on = false;
bool lights_2_request_on = false;
bool door_magnet_1_request_on = false;
bool door_magnet_2_request_on = false;
bool heater_1_force_off = false;
bool heater_2_force_off = false;
bool heater_3_force_off = false;
bool lights_1_force_off = false;
bool lights_2_force_off = false;
bool door_magnet_1_force_off = false;
bool door_magnet_2_force_off = false;

// DI Variables
bool door_is_closed = false;
bool door_is_open = false;
int door_sensor_raw = 0;

// Sensor Variables
float floor_temperature = 0.0;
float bench_temperature = 0.0;
float ceiling_temperature = 0.0;
float sea_temperature = 0.0;
float outside_temperature_1 = 0.0;
float outside_temperature_2 = 0.0;

// Status Variables
bool door_error = false;
bool temperature_sensor_error = false;
bool heater_error = false;
bool network_error = false;
bool general_error = false;
int door_closed_since = 0;
int door_open_since = 0;
int heater_1_on_duration = 0;
int heater_2_on_duration = 0;
int heater_3_on_duration = 0;

// Config Variables
int door_open_heater_off_deadline = 0;
int heater_on_max_periode = 0;
float heater_shutdown_temperature = 0.0;

Variable variables[] = {
    {"heater_1_request_on", BOOL, &heater_1_request_on, true},
    {"heater_2_request_on", BOOL, &heater_2_request_on, true},
    {"heater_3_request_on", BOOL, &heater_3_request_on, true},
    {"lights_1_request_on", BOOL, &lights_1_request_on, true},
    {"lights_2_request_on", BOOL, &lights_2_request_on, true},
    {"door_magnet_1_request_on", BOOL, &door_magnet_1_request_on, true},
    {"door_magnet_2_request_on", BOOL, &door_magnet_2_request_on, true},
    {"heater_1_force_off", BOOL, &heater_1_force_off, true},
    {"heater_2_force_off", BOOL, &heater_2_force_off, true},
    {"heater_3_force_off", BOOL, &heater_3_force_off, true},
    {"lights_1_force_off", BOOL, &lights_1_force_off, true},
    {"lights_2_force_off", BOOL, &lights_2_force_off, true},
    {"door_magnet_1_force_off", BOOL, &door_magnet_1_force_off, true},
    {"door_magnet_2_force_off", BOOL, &door_magnet_2_force_off, true},
    {"door_is_closed", BOOL, &door_is_closed, true},
    {"door_is_open", BOOL, &door_is_open, true},
    {"door_sensor_raw", INT, &door_sensor_raw, true},
    {"floor_temperature", FLOAT, &floor_temperature, true},
    {"bench_temperature", FLOAT, &bench_temperature, true},
    {"ceiling_temperature", FLOAT, &ceiling_temperature, true},
    {"sea_temperature", FLOAT, &sea_temperature, true},
    {"outside_temperature_1", FLOAT, &outside_temperature_1, true},
    {"outside_temperature_2", FLOAT, &outside_temperature_2, true},
    {"door_error", BOOL, &door_error, true},
    {"temperature_sensor_error", BOOL, &temperature_sensor_error, true},
    {"heater_error", BOOL, &heater_error, true},
    {"network_error", BOOL, &network_error, true},
    {"general_error", BOOL, &general_error, true},
    {"door_closed_since", INT, &door_closed_since, true},
    {"door_open_since", INT, &door_open_since, true},
    {"heater_1_on_duration", INT, &heater_1_on_duration, true},
    {"heater_2_on_duration", INT, &heater_2_on_duration, true},
    {"heater_3_on_duration", INT, &heater_3_on_duration, true},
    {"door_open_heater_off_deadline", INT, &door_open_heater_off_deadline, true},
    {"heater_on_max_periode", INT, &heater_on_max_periode, true},
    {"heater_shutdown_temperature", FLOAT, &heater_shutdown_temperature, true},
};

void publish_variables() {
    for (size_t i = 0; i < sizeof(variables) / sizeof(variables[0]); ++i) {
        if (variables[i].publish) {
            char payload[50];
            switch (variables[i].type) {
                case BOOL:
                    snprintf(payload, sizeof(payload), "%s", (*(bool *)variables[i].value) ? "true" : "false");
                    break;
                case INT:
                    snprintf(payload, sizeof(payload), "%d", *(int *)variables[i].value);
                    break;
                case FLOAT:
                    snprintf(payload, sizeof(payload), "%.2f", *(float *)variables[i].value);
                    break;
            }
            mqtt_publish(variables[i].name, payload); // Assuming you have an mqtt_publish function
        }
    }
}

void update_variable(const char *name, const char *value) {
    for (size_t i = 0; i < sizeof(variables) / sizeof(variables[0]); ++i) {
        if (strcmp(variables[i].name, name) == 0) {
            switch (variables[i].type) {
                case BOOL:
                    *(bool *)variables[i].value = (strcmp(value, "true") == 0);
                    break;
                case INT:
                    *(int *)variables[i].value = atoi(value);
                    break;
                case FLOAT:
                    *(float *)variables[i].value = atof(value);
                    break;
            }
            break;
        }
    }
}