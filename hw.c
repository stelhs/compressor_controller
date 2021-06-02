#include "hw.h"

#include <stdio.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include "gpio.h"
#include "uart.h"
#include "sys_timer.h"
#include "leds.h"
#include "main.h"


/**
 * config for all used GPIO
 */
struct gpio gpio_list[] = {
        { // MCU_GPIO_MOTOR
            .direction_addr = (u8 *) &DDRC,
            .port_addr = (u8 *) &PORTC,
            .pin = 1,
            .direction = GPIO_OUTPUT,
            .output_state = 0
        },
        { // MCU_GPIO_VALVE
            .direction_addr = (u8 *) &DDRC,
            .port_addr = (u8 *) &PORTC,
            .pin = 2,
            .direction = GPIO_OUTPUT,
            .output_state = 0
        },
        { // MCU_GPIO_ROTATE_SENSOR
            .direction_addr = (u8 *) &DDRD,
            .port_addr = (u8 *) &PORTD,
            .pin_addr = (u8 *) &PIND,
            .pin = 2,
            .direction = GPIO_INPUT,
            .output_state = 1
        },
        { // MCU_GPIO_PRESSURE_SENSOR
            .direction_addr = (u8 *) &DDRC,
            .port_addr = (u8 *) &PORTC,
            .pin_addr = (u8 *) &PINC,
            .pin = 0,
            .direction = GPIO_INPUT,
            .output_state = 1
        },
        { // MCU_GPIO_LED_GREEN
            .direction_addr = (u8 *) &DDRD,
            .port_addr = (u8 *) &PORTD,
            .pin = 5,
            .direction = GPIO_OUTPUT,
            .output_state = 0
        },
        { // MCU_GPIO_LED_RED
            .direction_addr = (u8 *) &DDRD,
            .port_addr = (u8 *) &PORTD,
            .pin = 3,
            .direction = GPIO_OUTPUT,
            .output_state = 0
        },
        {
            .direction_addr = NULL,
            .port_addr = NULL,
        },
};

struct led leds[2];

void led_indicator_set_state(enum led_indicator_state state)
{
    led_off(leds + LED_GREEN);
    led_off(leds + LED_RED);

    switch (state) {
    case LI_OFF:
        led_off(leds + LED_GREEN);
        led_off(leds + LED_RED);
        break;

    case LI_STARTING_FIRST:
        led_off(leds + LED_RED);
        led_set_blink(leds + LED_GREEN, 100, 0, 0);
        break;

    case LI_STARTING_SECOND:
        led_off(leds + LED_GREEN);
        led_set_blink(leds + LED_RED, 100, 0, 0);
        break;

    case LI_RUNNING:
        led_off(leds + LED_RED);
        led_set_blink(leds + LED_GREEN, 500, 0, 0);
        break;

    case LI_WAITING:
        led_off(leds + LED_RED);
        led_on(leds + LED_GREEN);
        break;

    case LI_STARTING_ERROR:
        led_off(leds + LED_GREEN);
        led_on(leds + LED_RED);
        break;

    case LI_RUN_ERROR:
        led_off(leds + LED_GREEN);
        led_set_blink(leds + LED_RED, 500, 0, 0);
        break;
    }
}

SIGNAL(SIG_INTERRUPT0)
{
    gpio_int0_cb();
}


static void
init_gpio_int0(void)
{
    TIMSK = 1 << TOIE0;
    GIMSK = 1 << INT0;        // разрешить внешнее прерывание Int0 или 0x40
    MCUCR = 1 << ISC01;  // сигнал низкого уровня на Int0 вызовет внешнее прерывание
}


void
init_hw(void)
{
    gpio_init_list(gpio_list);
    usart_init();
    init_gpio_int0();
    sys_timer_init();
    led_register(leds + LED_GREEN, gpio_list + MCU_GPIO_LED_GREEN);
    led_register(leds + LED_RED, gpio_list + MCU_GPIO_LED_RED);
    wdt_enable(WDTO_2S); // Включаем вэтчдог
    sei();
}

