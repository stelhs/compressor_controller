#include <stdio.h>
#include <avr/wdt.h>
#include "uart.h"
#include "main.h"
#include "sys_timer.h"
#include "leds.h"
#include "hw.h"

volatile int rotate_cnt = 0;
volatile bool motor_rotate_freq = 0;
volatile int timer_sec_cnt = 0;
bool motor_enabled = 0;
struct sys_timer sys_timer;

static bool is_high_pressure(void)
{
    return gpio_get_value(gpio_list + MCU_GPIO_PRESSURE_SENSOR);
}


void sys_timer_cb(void *priv)
{
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
    if (motor_rotate_freq < 25)
        return FALSE;
    return TRUE;
}


static void motor_run(void)
{
    gpio_set_value(gpio_list + MCU_GPIO_MOTOR, 1);
    motor_enabled = 1;
}

static void motor_stop(void)
{
    gpio_set_value(gpio_list + MCU_GPIO_MOTOR, 0);
    motor_enabled = 0;
}

static bool is_motor_running(void)
{
    return motor_enabled;
}

static void valve_open(void)
{
    gpio_set_value(gpio_list + MCU_GPIO_VALVE, 1);
}

static void valve_close(void)
{
    gpio_set_value(gpio_list + MCU_GPIO_VALVE, 0);
}

static void debug_console(void)
{
    char key;
    key = usart_get();
    switch (key) {
    case 'a':
        printf("motor_rotate_freq = %d\r\n", motor_rotate_freq);
        break;

    }
}

static int handle_high_pressure(void)
{
    if (is_high_pressure() && is_motor_running()) {
        TIMEOUT(1000) {
            wdt_reset();
        }
        if (!is_high_pressure())
            return 0;

        printf("high pressure, stoped\r\n");
        motor_stop();
        valve_close();
        led_indicator_set_state(LI_WAITING);
        TIMEOUT(3000) {
            wdt_reset();
        }

        return 1;
    }
    return 0;
}

static int motor_start(void)
{
    #define MOTOR_ROTATE_TIMEOUT 7000
    #define MOTOR_START_ATTEMPTS_CNT 5

    static struct timeout start_timeout;
    int attempts = MOTOR_START_ATTEMPTS_CNT;

    printf("motor starting...\r\n");

    led_indicator_set_state(LI_STARTING_FIRST);

    TIMEOUT(15000) {
        if (handle_high_pressure())
            return 0;
        wdt_reset();
    }

    timeout_start(&start_timeout, MOTOR_ROTATE_TIMEOUT);
    motor_run();
    valve_open();
    for (;;) {
        wdt_reset();
        if (is_timeout_expire(&start_timeout)) {
            motor_stop();
            valve_close();
            led_indicator_set_state(LI_STARTING_SECOND);
            printf("Can't start motor\r\n");

            attempts --;
            if (attempts == 0) {
                return -1;
            }
            printf("attempts = %d\r\n",
                    MOTOR_START_ATTEMPTS_CNT - attempts);

            TIMEOUT(60000) {
                if (handle_high_pressure())
                    return 0;
                wdt_reset();
            }

            timeout_start(&start_timeout, MOTOR_ROTATE_TIMEOUT);
            valve_open();
            motor_run();
        }

        if (handle_high_pressure()) {
            led_indicator_set_state(LI_WAITING);
            return 0;
        }

        if (is_motor_rotate()) {
            led_indicator_set_state(LI_RUNNING);
            printf("motor success started\r\n");
            valve_close();
            return 0;
        }
    }
    return 0;
}

int main(void)
{
    int rc;
    init_hw();
    sys_timer.devisor = 1;
    sys_timer.handler = sys_timer_cb;
    sys_timer_add_handler(&sys_timer);

    printf("Init - ok\r\n");
    led_indicator_set_state(LI_WAITING);

    for (;;) {
        wdt_reset();
        debug_console();

        if (!is_high_pressure() && !is_motor_running()) {
            rc = motor_start();
            if (rc < 0) {
                led_indicator_set_state(LI_STARTING_ERROR);
                printf("Attempts expired\r\n");
                for(;;)
                    wdt_reset();
            }
        }

        if (is_motor_running() && !is_motor_rotate()) {
            motor_stop();
            valve_close();
            led_indicator_set_state(LI_RUN_ERROR);
            printf("Error: motor is not rotating\r\n");
            for(;;)
                wdt_reset();
        }


        handle_high_pressure();
    }
}





