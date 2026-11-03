#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "button.h"
#include "config.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"

static const char *TAG = "lesson_2_3";

enum {
    // Give the idle task one tick per iteration so the task watchdog stays happy.
    // With CONFIG_FREERTOS_HZ=1000 (sdkconfig.defaults) one tick is 1 ms.
    LOOP_YIELD_TICKS = 1,
};

// LED modes, switched by the button: Blinking -> Always On -> Always Off -> Blinking.
typedef enum {
    MODE_BLINKING = 0,
    MODE_ALWAYS_ON,
    MODE_ALWAYS_OFF,
} led_mode_t;

// Superloop timing accumulated over APP_CONFIG.stats_every_n_loops iterations.
typedef struct {
    uint64_t work_sum_us; // total time spent doing work (without the yield)
    uint32_t work_min_us;
    uint32_t work_max_us;
    uint32_t count;
    uint64_t window_start_us; // to compute the full iteration period
} loop_stats_t;

static led_mode_t mode_next(led_mode_t mode)
{
    switch (mode) {
    case MODE_BLINKING:
        return MODE_ALWAYS_ON;
    case MODE_ALWAYS_ON:
        return MODE_ALWAYS_OFF;
    case MODE_ALWAYS_OFF:
    default:
        return MODE_BLINKING;
    }
}

static const char *mode_name(led_mode_t mode)
{
    switch (mode) {
    case MODE_BLINKING:
        return "Blinking";
    case MODE_ALWAYS_ON:
        return "Always On";
    case MODE_ALWAYS_OFF:
        return "Always Off";
    default:
        return "?";
    }
}

// Put the LED in the starting state of `mode`.
static void mode_enter(led_t *led, led_mode_t mode, uint64_t now_us, uint64_t *last_toggle_us)
{
    switch (mode) {
    case MODE_BLINKING:
        led_set(led, LED_ON);
        *last_toggle_us = now_us;
        break;
    case MODE_ALWAYS_ON:
        led_set(led, LED_ON);
        break;
    case MODE_ALWAYS_OFF:
    default:
        led_set(led, LED_OFF);
        break;
    }
    ESP_LOGI(TAG, "Mode: %s", mode_name(mode));
}

static void stats_reset(loop_stats_t *stats, uint64_t now_us)
{
    stats->work_sum_us = 0;
    stats->work_min_us = UINT32_MAX;
    stats->work_max_us = 0;
    stats->count = 0;
    stats->window_start_us = now_us;
}

static void stats_add(loop_stats_t *stats, uint32_t work_us)
{
    stats->work_sum_us += work_us;
    if (work_us < stats->work_min_us) {
        stats->work_min_us = work_us;
    }
    if (work_us > stats->work_max_us) {
        stats->work_max_us = work_us;
    }
    stats->count++;
}

static void stats_report(const loop_stats_t *stats, uint64_t now_us)
{
    char text[STATS_TEXT_SIZE];
    const uint64_t avg_us = stats->work_sum_us / stats->count;
    const uint64_t period_us = (now_us - stats->window_start_us) / stats->count;

    snprintf(text, sizeof(text),
             "loops=%" PRIu32 " work avg=%" PRIu64 " us min=%" PRIu32 " us max=%" PRIu32 " us | period=%" PRIu64 " us",
             stats->count, avg_us, stats->work_min_us, stats->work_max_us, period_us);
    ESP_LOGI(TAG, "%s", text);
}

void app_main(void)
{
    const uint64_t blink_interval_us = (uint64_t)APP_CONFIG.blink_interval_ms * US_PER_MS;
    const uint64_t debounce_us = (uint64_t)APP_CONFIG.debounce_ms * US_PER_MS;

    led_t led;
    led_mode_t mode = MODE_BLINKING;
    uint64_t last_toggle_us = 0;
    loop_stats_t stats;

    // Setup
    led_init(&led, APP_CONFIG.led_pin);
    button_init(APP_CONFIG.button_pin);
    stats_reset(&stats, (uint64_t)esp_timer_get_time());
    mode_enter(&led, mode, (uint64_t)esp_timer_get_time(), &last_toggle_us);

    // Superloop: nothing here blocks, every step just checks the time.
    while (1) {
        const uint64_t start_us = (uint64_t)esp_timer_get_time();

        // 1. Button: next mode on every debounced press
        if (button_poll(start_us, debounce_us)) {
            mode = mode_next(mode);
            mode_enter(&led, mode, start_us, &last_toggle_us);
        }

        // 2. LED: toggle when the blink interval has passed
        if (mode == MODE_BLINKING && (start_us - last_toggle_us) >= blink_interval_us) {
            led_toggle(&led);
            last_toggle_us += blink_interval_us; // keeps the rhythm exact, no drift
        }

        // 3. Timing: how long this iteration's work took
        const uint64_t end_us = (uint64_t)esp_timer_get_time();
        stats_add(&stats, (uint32_t)(end_us - start_us));
        if (stats.count >= APP_CONFIG.stats_every_n_loops) {
            stats_report(&stats, end_us);
            stats_reset(&stats, end_us);
        }

        vTaskDelay(LOOP_YIELD_TICKS);
    }
}
