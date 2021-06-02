#include <stdio.h>
#include "types.h"
#include "gpio.h"

/**
 * Установить режим GPIO
 * @param gpio - GPIO порт
 * @param mode - 0-вход, 1-выход
 */
void
gpio_set_direction(struct gpio *gpio, enum gpio_direction dir)
{
    if (dir)
        gpio->direction_addr[0] |= (1 << gpio->pin);
    else
        gpio->direction_addr[0] &= ~(1 << gpio->pin);
    gpio->direction = dir;
}

/**
 * Установить состояние выхода порта GPIO
 * @param gpio - GPIO порт
 * @param mode - ON или OFF
 */
void
gpio_set_value(struct gpio *gpio, u8 mode)
{
    if (mode)
        gpio->port_addr[0] |= (1 << gpio->pin);
    else
        gpio->port_addr[0] &= ~(1 << gpio->pin);

    gpio->output_state = mode;
}


/**
 * Получить текущее состояние входа порта GPIO
 * @param gpio - GPIO порт
 */
int
gpio_get_value(struct gpio *gpio)
{
    return ((gpio->pin_addr[0] & (1 << gpio->pin)) > 0);
}

/**
 * Инициализация списка GPIO портов
 * @param gpio_list
 */
void
gpio_init_list(struct gpio *gpio_list)
{
    struct gpio *gpio;
    for (gpio = gpio_list; gpio->direction_addr != NULL; gpio++) {
        gpio_set_direction(gpio, gpio->direction);
        gpio_set_value(gpio, gpio->output_state);
    }
}
