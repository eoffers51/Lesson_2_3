// Wokwi-only version of main/led.c
//
// wokwi.com compiles ESP-IDF code with the Arduino ESP32 core, which doesn't have
// the espressif/led_strip component. This file keeps the same led.h API but uses
// the core's built-in WS2812 function instead. The real firmware uses main/led.c.

#include "led.h"

#include "config.h"
#include "esp32-hal-rgb-led.h"

static void led_write(const led_t *led, led_state_t state)
{
    if (state == LED_ON) {
        const app_color_t *color = &APP_CONFIG.led_on_color;
        rgbLedWrite(led->pin, color->red, color->green, color->blue);
    } else {
        rgbLedWrite(led->pin, 0, 0, 0);
    }
}

void led_init(led_t *led, uint8_t pin)
{
    led->pin = pin;
    led->strip = NULL; // not used here
    led_set(led, LED_OFF);
}

void led_set(led_t *led, led_state_t state)
{
    led_write(led, state);
    led->state = state;
}

void led_toggle(led_t *led)
{
    led_set(led, (led->state == LED_ON) ? LED_OFF : LED_ON);
}

led_state_t led_get_state(const led_t *led)
{
    return led->state;
}
