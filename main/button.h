#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <stdint.h>

// Configure an active-LOW button on `pin` with a falling-edge interrupt.
void button_init(uint8_t pin);

// Call from the superloop every iteration. Returns true once per debounced press:
// after an interrupt edge, waits `debounce_us` and accepts the press only if the
// button is still held (so release bounces are ignored).
bool button_poll(uint64_t now_us, uint64_t debounce_us);

#endif // BUTTON_H
