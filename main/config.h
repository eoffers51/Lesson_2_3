#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdint.h>

// Color used when the LED is "on" (the onboard LED is an RGB WS2812).
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} app_color_t;

// All application settings in one place - no magic numbers in the code.
typedef struct {
    uint8_t led_pin;              // onboard WS2812 RGB LED
    uint8_t button_pin;           // onboard Boot button (active LOW)
    uint32_t blink_interval_ms;   // time between LED toggles in Blinking mode
    uint32_t debounce_ms;         // button must still be pressed this long after the edge
    uint32_t stats_every_n_loops; // print superloop timing every N iterations
    app_color_t led_on_color;     // dim blue, the WS2812 is very bright at full power
} app_config_t;

static const app_config_t APP_CONFIG = {
    .led_pin = 48,
    .button_pin = 0,
    .blink_interval_ms = 500,
    .debounce_ms = 50,
    .stats_every_n_loops = 1000,
    .led_on_color = {.red = 0, .green = 0, .blue = 40},
};

// Integer constants that are not settings.
enum {
    US_PER_MS = 1000,     // esp_timer_get_time() counts microseconds
    STATS_TEXT_SIZE = 128 // fixed buffer for the timing report
};

#endif // APP_CONFIG_H
