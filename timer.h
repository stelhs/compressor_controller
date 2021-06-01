#ifndef TIMER_H_
#define TIMER_H_

#include "list.h"

struct timer {
    struct le le;
    t_counter cnt;
};

bool is_timer_expire(struct timer *tmr);
void timer_start(struct timer *tmr, t_counter max);
void timer_stop(struct timer *tmr);
void timers_inc(void);

extern struct timer sleep_tmr;

#define TIMEOUT(msec) \
    for (timer_start(&sleep_tmr, msec); !is_timer_expire(&sleep_tmr); )



#endif /* TIMER_H_ */
