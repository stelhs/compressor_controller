/*
 * serial.c
 *
 *  Created on: 25.01.2014
 *      Author: Michail Kurochkin
 */

#include "uart.h"

#include <stdio.h>
#include <avr/io.h>

/**
 * Вывод в RS232 порт 0
 * @param var - символ для отправки
 */
static int
usart_put(char var)
{
    while (!(UCSRA & (1 << UDRE)));
    UDR = var;
    return 0;
}

/**
 * Чтение из RS232 порт 0
 * @return Ожидает и возвращает прочитанный символ
 */
static char
usart_get_blocked(void)
{
    while (!(UCSRA & (1 << RXC)));
    return UDR;
}

/**
 * Чтение из RS232 порт 0
 * @return возвращает символ из внутреннего буфера
 */
char
usart_get(void)
{
    if ((UCSRA & (1 << RXC)))
        return UDR;
    return 0;
}


/**
 * Инициалтизация RS232 порта 0
 * @param ubrr - скорость BAUD RATE
 */
void
usart_init(void)
{
    unsigned int ubrr = F_CPU / 16 / SERIAL_BAUD_RATE - 1;

    UBRRH = (unsigned char) (ubrr >> 8);
    UBRRL = (unsigned char) ubrr;
    UCSRB = (1 << RXEN) | (1 << TXEN);

    UCSRC = (1 << URSEL) | (3 << UCSZ0);
    fdevopen((void *) usart_put, (void *) usart_get_blocked);
}

