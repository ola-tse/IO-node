cmake_minimum_required(VERSION 3.13.1)
set(DTC_OVERLAY_FILE "/Users/olaoren/01_code/zephyrproject/projects/IO-node/src/dts/stm32h723zg_ds18b20.overlay")
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(IO-node)

target_sources(app PRIVATE 
    src/main.c
    src/sensors/ds18b20.c
    src/network/general.c
    src/DO/DO.c
    src/network/mqtt_client.c
    #src/network/mqtt_handlers.c
    src/common/variables.c)