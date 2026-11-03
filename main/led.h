#ifndef LED_H
#define LED_H

#include <stdint.h>

typedef enum {
    LED_OFF = 0,
    LED_ON,
} led_state_t;

// Driver handle type from led_strip; declared here so users of led.h
// don't need to include the driver headers.
struct led_strip_t;

typedef struct {
    uint8_t pin;
    led_state_t state;
    struct led_strip_t *strip;
} led_t;

// Set up the WS2812 LED on `pin` and switch it off.
void led_init(led_t *led, uint8_t pin);

// Switch the LED on or off.
void led_set(led_t *led, led_state_t state);

// Switch to the opposite state.
void led_toggle(led_t *led);

led_state_t led_get_state(const led_t *led);

#endif // LED_H
