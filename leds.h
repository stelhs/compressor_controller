#ifndef LEDS_H
#define LEDS_H

#include "types.h"
#include "sys_timer.h"

struct led
{
	struct gpio *gpio;

    volatile u8 light_state : 1; // Led light? 1 or 0
	volatile u8 state : 1; // current state
	volatile t_counter active_interval;
	volatile t_counter inactive_interval;

	volatile t_counter timer_counter;
	volatile t_counter blink_counter;

	struct sys_timer timer;
};

void led_register(struct led *led, struct gpio *gpio, bool light_state);

void led_on(struct led *led);
void led_off(struct led *led);
void led_set_blink(struct led *led, t_counter interval1,
			t_counter interval2, int count);

#endif
