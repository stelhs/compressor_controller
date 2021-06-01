#ifndef LED_LIB_H
#define LED_LIB_H

#include "types.h"

enum gpio_direction {
    GPIO_INPUT,
    GPIO_OUTPUT
};

struct gpio {
    u8 *direction_addr;
    u8 *port_addr;
    u8 *pin_addr;
    u8 pin;
    enum gpio_direction direction;
    u8 output_state :1; // Установленное состояние порта
};

void
gpio_init_list(struct gpio *gpio_list);

void
gpio_set_direction(struct gpio *gpio, u8 mode);

void
gpio_set_state(struct gpio *gpio, u8 mode);

int
gpio_get_state(struct gpio *gpio);


#endif
