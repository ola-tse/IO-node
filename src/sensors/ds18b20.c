// File: src/ds18b20.c
#include "ds18b20.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define W1_BUS_NODE DT_NODELABEL(w1_0)
#define DS18B20_NODE DT_NODELABEL(temp_sens_ds18b20)

const struct device *get_ds18b20_device(void)
{
	const struct device *const dev = DEVICE_DT_GET_ANY(maxim_ds18b20);

	if (dev == NULL) {
		/* No such node, or the node does not have status "okay". */
		printk("\nError: no device found.\n");
		return NULL;
	}

	if (!device_is_ready(dev)) {
		printk("\nError: Device \"%s\" is not ready; "
		       "check the driver initialization logs for errors.\n",
		       dev->name);
		return NULL;
	}

	printk("Found device \"%s\", getting sensor data\n", dev->name);
	return dev;
}