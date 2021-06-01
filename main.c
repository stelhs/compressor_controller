#include <stdio.h>
#include <avr/wdt.h>
#include "init_hw.h"
#include "uart.h"
#include "main.h"
#include "timer.h"

volatile int rotate_cnt = 0;
volatile bool motor_rotate_freq = 0;
volatile int timer_sec_cnt = 0;
bool motor_enabled = 0;

static bool is_high_pressure(void)
{
    return gpio_get_state(gpio_list + MCU_GPIO_PRESSURE_SENSOR);
}


void timer2_cb(void)
{
    timers_inc();
    timer_sec_cnt ++;
    if (timer_sec_cnt >= 1000) {
        timer_sec_cnt = 0;
        motor_rotate_freq = rotate_cnt;
        rotate_cnt = 0;
    }
}

void gpio_int0_cb(void)
{
    rotate_cnt ++;
}


static bool is_motor_rotate(void)
{
    if (motor_rotate_freq < 5)
        return FALSE;
    return TRUE;
}


static void motor_run(void)
{
    gpio_set_state(gpio_list + MCU_GPIO_MOTOR, 1);
    motor_enabled = 1;
}

static void motor_stop(void)
{
    gpio_set_state(gpio_list + MCU_GPIO_MOTOR, 0);
    motor_enabled = 0;
}

static bool is_motor_running(void)
{
    return motor_enabled;
}

static void valve_open(void)
{
    gpio_set_state(gpio_list + MCU_GPIO_VALVE, 0);
}

static void valve_close(void)
{
    gpio_set_state(gpio_list + MCU_GPIO_VALVE, 1);
}

static void debug_console(void)
{
    char key;
    key = usart_get();
    switch (key) {
    case 'a':
        printf("motor run\r\n");
        motor_run();
        break;

    case 's':
        printf("motor stop\r\n");
        motor_stop();
        break;

    case 'd':
        printf("valve open\r\n");
        valve_open();
        break;

    case 'f':
        printf("valve close\r\n");
        valve_close();
        break;
    }
}

static int handle_high_pressure(void)
{
    if (is_high_pressure() && is_motor_running()) {
        printf("high pressure, stoped\r\n");
        motor_stop();
        valve_close();
        return 1;
    }
    return 0;
}

static int motor_start(void)
{
    static struct timer start_timeout;
    int attempts = 5;
    #define MOTOR_ROTATE_TIMEOUT 3000

    printf("waiting 5000 ...\r\n");
    TIMEOUT(5000) {
        if (handle_high_pressure())
            return 0;
        wdt_reset();
    }

    printf("motor starting...\r\n");
    timer_start(&start_timeout, MOTOR_ROTATE_TIMEOUT);
    motor_run();
    valve_open();
    for (;;) {
        wdt_reset();
        if (is_timer_expire(&start_timeout)) {
            motor_stop();
            valve_close();
            printf("timer expire, sleep 10000\r\n");
            TIMEOUT(10000) {
                if (handle_high_pressure())
                    return 0;
                wdt_reset();
            }

            attempts --;
            if (attempts == 0)
                return -1;

            printf("attempts = %d\r\n", attempts);
            timer_start(&start_timeout, MOTOR_ROTATE_TIMEOUT);
            valve_open();
            motor_run();
        }

        if (handle_high_pressure())
            return 0;

        if (is_motor_rotate()) {
            printf("motor success started\r\n");
            valve_close();
            return 0;
        }
    }
}

int main(void)
{
    int rc;
    init_hw();
    printf("Init - ok\r\n");

    for (;;) {
        wdt_reset();
        //debug_console();

        if (!is_high_pressure() && !is_motor_running()) {
            rc = motor_start();
            if (rc < 0)
                printf("Attempts expired\r\n");
                for(;;)
                    wdt_reset();
        }

        if (is_motor_running() && !is_motor_rotate()) {
            motor_stop();
            valve_close();
            printf("Error: motor is not rotating\r\n");
            for(;;)
                wdt_reset();
        }


        handle_high_pressure();
    }
}





