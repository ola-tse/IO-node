build cmd: 

west build -b nucleo_h723zg -- -DDTC_OVERLAY_FILE="/Users/olaoren/01_code/zephyrproject/projects/IO-node/src/dts/stm32h723zg_ds18b20.overlay" -DCONF_FILE=prj.conf

minicom -D /dev/tty.usbmodem11103 -b 115200


#DO 

heater_1_request_on
heater_2_request_on
heater_3_request_on

lights_1_request_on
lights_2_request_on

door_magnet_1_request_on
door_magnet_2_request_on

heater_1_force_off
heater_2_force_off
heater_3_force_off

lights_1_force_off
lights_2_force_off

door_magnet_1_force_off
door_magnet_2_force_off

#DI

door_is_closed
door_is_open
door_sensor_raw


#Senors

floor_temperature
bench_temperature
ceiling_temperature
sea_temperature
outside_temperature_1
outside_temperature_2

#Status

door_error
temperature_sensor_error
heater_error
network_error
general_error

door_closed_since
door_open_since

heater_1_on_duration
heater_2_on_duration
heater_3_on_duration

#Configs

door_open_heater_off_deadline
heater_on_max_periode
heater_shutdown_temperature

