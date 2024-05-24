// File: src/ds18b20.h
#ifndef DS18B20_H
#define DS18B20_H

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

//int ds18b20_init();
const struct device *get_ds18b20_device();


#endif // DS18B20_H