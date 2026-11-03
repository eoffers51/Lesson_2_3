#include "led.h"

#include "config.h"
#include "esp_err.h"
#include "led_strip.h"

enum {
    // The ESP32 drives the WS2812 with its RMT peripheral. 10 MHz = 0.1 us steps,
    // fine enough to build the WS2812 pulses.
    LED_RMT_RESOLUTION_HZ = 10 * 1000 * 1000,
    LED_COUNT = 1, // one LED on the board
    LED_INDEX = 0, // ...and this is it
};

void led_init(led_t *led, uint8_t pin)
{
    const led_strip_config_t strip_config = {
        .strip_gpio_num = pin,
        .max_leds = LED_COUNT,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
    };
    const led_strip_rmt_config_t rmt_config = {
        .resolution_hz = LED_RMT_RESOLUTION_HZ,
    };

    led->pin = pin;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led->strip));
    led_set(led, LED_OFF);
}

void led_set(led_t *led, led_state_t state)
{
    if (state == LED_ON) {
        const app_color_t *color = &APP_CONFIG.led_on_color;
        ESP_ERROR_CHECK(led_strip_set_pixel(led->strip, LED_INDEX, color->red, color->green, color->blue));
        ESP_ERROR_CHECK(led_strip_refresh(led->strip));
    } else {
        ESP_ERROR_CHECK(led_strip_clear(led->strip));
    }
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
