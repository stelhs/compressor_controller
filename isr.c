#include <avr/interrupt.h>
#include "main.h"

/**
 * timer 2 callback. Frequency: 1000Hz
 */
SIGNAL(SIG_OUTPUT_COMPARE2)
{
    timer2_cb();
}

SIGNAL(SIG_INTERRUPT0)
{
    gpio_int0_cb();
}
