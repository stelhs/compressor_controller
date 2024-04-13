/*
 * sys_timer.c
 *
 *  Created on: 07 июля 2016 г.
 *      Author: Michail Kurochkin
 */

#include <avr/interrupt.h>
#include "types.h"
#include "list.h"
#include "sys_timer.h"


static struct list list_subscribers = LIST_INIT;
static struct list timeout_subscribers = LIST_INIT;
struct timeout sleep_tmo = {LIST_INIT, 0};


static inline void
timeouts_inc(void)
{
    struct le *le;
    struct timeout *removed_tmo = NULL;
    LIST_FOREACH(&timeout_subscribers, le) {
        struct timeout *tmo = list_ledata(le);
        if (tmo->cnt > 0) {
            tmo->cnt --;
            continue;
        }

        if (tmo->cnt == 1) {
            removed_tmo = tmo;
        }
    }

    if (removed_tmo)
        list_unlink(&removed_tmo->le);
}



/**
 * timer 2 callback. Frequency: 1000Hz
 */
SIGNAL(SIG_OUTPUT_COMPARE2)
{
	struct le *le;

	timeouts_inc();

	LIST_FOREACH(&list_subscribers, le) {
		struct sys_timer *timer = list_ledata(le);

		timer->cnt++;
		if (timer->cnt == timer->devisor) {
			timer->handler(timer->priv);
			timer->cnt = 0;
		}
	}
}

void sys_timer_add_handler(struct sys_timer *timer)
{
	cli();
	timer->cnt = 0;
	list_append(&list_subscribers, &timer->le, timer);
	sei();
}

void
sys_timer_init(void)
{
    // configuring timer 2: Frequency: 1000Hz
    OCR2 = (u8)((u32)F_CPU / 1000 / (2 * 32) - 1);
    TCCR2 = 0b011;
    TIFR |= OCF2;
    TIMSK |= _BV(OCIE2);
}

bool is_timeout_expire(struct timeout *tmo)
{
    bool expire;
    cli();
    expire = (tmo->cnt <= 1);
    sei();
    return expire;
}

void timeout_start(struct timeout *tmo, t_counter max)
{
    struct le *le;
    tmo->cnt = max + 1;
    cli();
    LIST_FOREACH(&timeout_subscribers, le) {
        struct timeout *t = list_ledata(le);
        if (t == tmo) {
            t->cnt = max;
            sei();
            return;
        }
    }
    list_append(&timeout_subscribers, &tmo->le, tmo);
    sei();
}

void timeout_stop(struct timeout *tmo)
{
    cli();
    tmo->cnt = 0;
    list_unlink(&tmo->le);
    sei();
}




