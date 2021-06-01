#include <stdio.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include "gpio.h"
#include "init_hw.h"
#include "uart.h"


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
        {
            .direction_addr = NULL,
            .port_addr = NULL,
        },
};


static void
init_gpio_int0(void)
{
    TIMSK = 1 << TOIE0;
    GIMSK = 1 << INT0;        // разрешить внешнее прерывание Int0 или 0x40
    MCUCR = 1 << ISC01;  // сигнал низкого уровня на Int0 вызовет внешнее прерывание
}

static void
timer2_init(void)
{
    // configuring timer 2: Frequency: 1000Hz
    OCR2 = (u8)((u32)F_CPU / 1000 / (2 * 32) - 1);
    TCCR2 = 0b011;
    TIFR |= OCF2;
    TIMSK |= _BV(OCIE2);
}

void
init_hw(void)
{
    gpio_init_list(gpio_list);
    usart_init();
    init_gpio_int0();
    timer2_init();
    wdt_enable(WDTO_2S); // Включаем вэтчдог
    sei();
}

