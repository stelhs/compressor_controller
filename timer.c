#include <avr/interrupt.h>
#include "list.h"
#include "timer.h"

static struct list timer_subscribers = LIST_INIT;
struct timer sleep_tmr = {LIST_INIT, 0};

bool is_timer_expire(struct timer *tmr)
{
    bool expire;
    cli();
    expire = (tmr->cnt == 1);
    sei();
    return expire;
}

void timer_start(struct timer *tmr, t_counter max)
{
    struct le *le;
    tmr->cnt = max + 1;
    cli();
    LIST_FOREACH(&timer_subscribers, le) {
        struct timer *t = list_ledata(le);
        if (t == tmr) {
            t->cnt = max;
            sei();
            return;
        }
    }
    list_append(&timer_subscribers, &tmr->le, tmr);
    sei();
}

void timer_stop(struct timer *tmr)
{
    cli();
    tmr->cnt = 0;
    list_unlink(&tmr->le);
    sei();
}

void timers_inc(void)
{
    struct le *le;
    struct timer *removed_tmr = NULL;
    LIST_FOREACH(&timer_subscribers, le) {
        struct timer *tmr = list_ledata(le);
        if (tmr->cnt > 0) {
            tmr->cnt --;
            continue;
        }

        if (tmr->cnt == 1) {
            removed_tmr = tmr;
        }
    }

    if (removed_tmr)
        list_unlink(&removed_tmr->le);
}

