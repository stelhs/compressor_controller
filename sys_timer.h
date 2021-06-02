#ifndef SYS_TIMER_H_
#define SYS_TIMER_H_

#include "list.h"

struct sys_timer {
	int devisor;
	void *priv;
	void (*handler)(void *);
// Private:
	struct le le;
	int cnt;
};


struct timeout {
    struct le le;
    t_counter cnt;
};

void sys_timer_add_handler(struct sys_timer *timer);
void sys_timer_init(void);

bool is_timeout_expire(struct timeout *);
void timeout_start(struct timeout *, t_counter max);
void timeout_stop(struct timeout *);

extern struct timeout sleep_tmo;

#define TIMEOUT(msec) \
    for (timeout_start(&sleep_tmo, msec); !is_timeout_expire(&sleep_tmo); )

#endif /* SYS_TIMER_H_ */
