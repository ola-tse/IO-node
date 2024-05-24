#ifndef VARIABLES_H
#define VARIABLES_H
#include <stdbool.h>

#include <stdbool.h>

typedef enum {
    INT,
    FLOAT,
    BOOL
} VariableType;

typedef struct {
    const char *name;
    VariableType type;
    void *value;
    bool publish;
} Variable;

extern Variable variables[];

// DO Variables
extern bool heater_1_request_on;
extern bool heater_2_request_on;
extern bool heater_3_request_on;
extern bool lights_1_request_on;
extern bool lights_2_request_on;
extern bool door_magnet_1_request_on;
extern bool door_magnet_2_request_on;
extern bool heater_1_force_off;
extern bool heater_2_force_off;
extern bool heater_3_force_off;
extern bool lights_1_force_off;
extern bool lights_2_force_off;
extern bool door_magnet_1_force_off;
extern bool door_magnet_2_force_off;

// DI Variables
extern bool door_is_closed;
extern bool door_is_open;
extern int door_sensor_raw;

// Sensor Variables
extern float floor_temperature;
extern float bench_temperature;
extern float ceiling_temperature;
extern float sea_temperature;
extern float outside_temperature_1;
extern float outside_temperature_2;

// Status Variables
extern bool door_error;
extern bool temperature_sensor_error;
extern bool heater_error;
extern bool network_error;
extern bool general_error;
extern int door_closed_since;
extern int door_open_since;
extern int heater_1_on_duration;
extern int heater_2_on_duration;
extern int heater_3_on_duration;

// Config Variables
extern int door_open_heater_off_deadline;
extern int heater_on_max_periode;
extern float heater_shutdown_temperature;


#endif // VARIABLES_H
