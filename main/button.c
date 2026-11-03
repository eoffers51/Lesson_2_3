#include "button.h"

#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

enum {
    BUTTON_LEVEL_PRESSED = 0, // active LOW: pressed pulls the pin to GND
    BUTTON_ISR_FLAGS = 0,     // default interrupt allocation flags
};

// Set by the ISR, read-and-cleared by the main loop.
static volatile bool button_pressed = false;
// Protects button_pressed so the main loop's read-and-clear can't lose an edge.
static portMUX_TYPE s_lock = portMUX_INITIALIZER_UNLOCKED;

// Module state (file scope, only reachable through the functions below).
static gpio_num_t s_pin;
static bool s_waiting = false; // an edge was seen, waiting for the debounce time
static uint64_t s_edge_us = 0; // when that edge was seen

// Keep the ISR minimal: just record that an edge happened.
static void IRAM_ATTR button_isr(void *arg)
{
    (void)arg;
    portENTER_CRITICAL_ISR(&s_lock);
    button_pressed = true;
    portEXIT_CRITICAL_ISR(&s_lock);
}

// Atomic read-and-clear: interrupts are masked while we copy and reset the flag.
static bool button_take_flag(void)
{
    portENTER_CRITICAL(&s_lock);
    const bool pressed = button_pressed;
    button_pressed = false;
    portEXIT_CRITICAL(&s_lock);
    return pressed;
}

void button_init(uint8_t pin)
{
    const gpio_config_t config = {
        .pin_bit_mask = 1ULL << pin,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE, // HIGH -> LOW = button pressed
    };

    s_pin = (gpio_num_t)pin;
    ESP_ERROR_CHECK(gpio_config(&config));
    ESP_ERROR_CHECK(gpio_install_isr_service(BUTTON_ISR_FLAGS));
    ESP_ERROR_CHECK(gpio_isr_handler_add(s_pin, button_isr, NULL));
}

bool button_poll(uint64_t now_us, uint64_t debounce_us)
{
    // Always take the flag, so bounces during the wait are consumed and dropped.
    if (button_take_flag() && !s_waiting) {
        s_waiting = true;
        s_edge_us = now_us;
    }

    if (!s_waiting || (now_us - s_edge_us) < debounce_us) {
        return false;
    }

    // Debounce time is over: it's a real press only if the button is still held.
    s_waiting = false;
    return gpio_get_level(s_pin) == BUTTON_LEVEL_PRESSED;
}
