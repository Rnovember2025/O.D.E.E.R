/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"

// Perform initialisation
int pico_led_init(void) {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
}

int main() {
    int pwm_period=10;
    int pwm_on_time=0;
    int fade_period=100/pwm_period;
    int fade_value=0;
    int fade_direction = 1;

    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    while (true) {
        fade_value++;

        if (fade_value >= fade_period) {
            fade_value = 0;

            if (pwm_on_time >= pwm_period) {
                fade_direction = -1;
            } else if (pwm_on_time <= 0) {
                fade_direction = 1;
            } else {
                fade_direction = fade_direction;
            }

            pwm_on_time = pwm_on_time + fade_direction;
        }
        
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(pwm_on_time);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(pwm_period-pwm_on_time);
    }
}
