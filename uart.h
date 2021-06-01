/*
 * serial.h
 *
 *  Created on: 25.01.2014
 *      Author: Michail Kurochkin
 */

#ifndef UART_H_
#define UART_H_

#define SERIAL_BAUD_RATE				9600

void
usart_init(void);

char
usart_get(void);

#endif /* UART_H_ */
