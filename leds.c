#include <avr/interrupt.h>
#include "types.h"
#include "gpio.h"
#include "leds.h"

static void led_timer_handler(void *arg)
{
	struct led *led = (struct led *)arg;

	if(!led->timer_counter)
		return;

	if(led->timer_counter > 1) {
		led->timer_counter--;
	    return;
	}

	if (led->blink_counter == 1) {
		gpio_set_value(led->gpio, 0);
		led->timer_counter = 0;
		led->blink_counter = 0;
		led->state = 0;
		return;
	}

	if(led->state) {
		gpio_set_value(led->gpio, 0);
		led->timer_counter = led->inactive_interval;
		led->state = 0;
	} else  {
		gpio_set_value(led->gpio, 1);
		led->timer_counter = led->active_interval;
		if (led->blink_counter > 1)
			led->blink_counter--;
		led->state = 1;
	}
}


/**
 * Register new led
 * @param led - struct allocated not in stack with one necessary parameter: gpio
 */
void led_register(struct led *led, struct gpio *gpio)
{
    led->gpio = gpio;
	gpio_set_value(gpio, 0);
	led->blink_counter = 0;
	led->active_interval = 0;
	led->inactive_interval = 0;

	led->timer.devisor = 10;
	led->timer.priv = led;
	led->timer.handler = led_timer_handler;
	sys_timer_add_handler(&led->timer);
}

/**
 * enable led light
 */
void led_on(struct led *led)
{
	gpio_set_value(led->gpio, ON);
    cli();
	led->active_interval = 0;
	led->inactive_interval = 0;
	led->blink_counter = 0;
	sei();
}

/**
 * disable led light
 */
void led_off(struct led *led)
{
	gpio_set_value(led->gpio, OFF);
    cli();
	led->active_interval = 0;
	led->inactive_interval = 0;
	led->blink_counter = 0;
    sei();
}

/**
 * set led blinking mode.
 * @param led - led descriptor
 * @param active_interval - light interval
 * @param inactive_interval - darkness interval
 * @param blink_count - blink count. 0 for not limited
 */
void led_set_blink(struct led *led, t_counter active_interval,
			t_counter inactive_interval, int blink_count)
{
	if(inactive_interval == 0)
	    inactive_interval = active_interval;

	gpio_set_value(led->gpio, 1);
	cli();
	led->active_interval = active_interval / 10 + 1;
	led->inactive_interval = inactive_interval / 10 + 1;
	led->blink_counter = active_interval;
	led->blink_counter = blink_count ? (blink_count + 1) : 0;
	sei();
}


