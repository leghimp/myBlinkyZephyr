/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   500

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

/* Get button node from devicetree alias "sw0" */
#define SW0_NODE DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void)
{
	int ret;
	bool led_state = true;
	/* Define GPIO spec from DT */
    	const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	if (!gpio_is_ready_dt(&button)) {
        printk("Error: button device %s is not ready\n", button.port->name);
        return 0;
    	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	/* Configure button as input */
    	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    	if (ret != 0) {
        	printk("Error %d: failed to configure %s pin %d\n",
                ret, button.port->name, button.pin);
        	return 0;
	}

	printk("Polling button on %s pin %d\n", button.port->name, button.pin);

	while (1) {
		/* Read button state */
		int val = gpio_pin_get_dt(&button);

		if (val < 0) {
			printk("Error %d: failed to read button\n", val);
		} else if (val > 0) {
			/* Button is held: Do nothing, just log it. 
			   The LED stays in whatever state it was last in. */
			printk("Button Pressed! Blinking suspended.\n");
		} else {
			ret = gpio_pin_toggle_dt(&led);
			if (ret < 0) {
				return 0;
			}
			led_state = !led_state;
		}

		printk("LED state: %s\n", led_state ? "LED ON" : "LED OFF");
		k_msleep(SLEEP_TIME_MS);
	}

	return 0;
}
