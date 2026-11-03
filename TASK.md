# Task: Embedded C Basics — Non-blocking Blink, Constants, and Interrupt-Driven Button

## Submission Format

* Code and execution results on GitHub.
* Link to the repository in the LMS.

## Embedded C Concepts

* `volatile`, `const`, `static` (file scope and function scope)
* `enum` and `static const` instead of `#define` magic numbers
* No dynamic memory (`malloc` / `free`); only statically allocated data
* Superloop architecture
* Modular design: header (`.h`) / source (`.c`) separation, opaque or explicit structs
* Safe work with peripherals (LED, button)

> Language standard: C11 (`-std=c11`). Do not use C++ features (classes, `constexpr`, `enum class`). If the Arduino framework is used, keep the logic in `.c` / `.h` files and call them from a thin `main.c` / `.ino` wrapper (`extern "C"` guards in headers).

## Requirements

### 1. Rewrite the "classic Arduino blink" in Embedded C

* Use a `typedef enum` for the LED state (`LED_OFF` / `LED_ON`).
* Use `static const` (or an anonymous `enum`) for the pin number and the blink interval. Use `#define` only for include guards.
* Avoid global variables. Keep necessary state `static` inside the `.c` file (file-scope) and expose it only through functions.
* Do not use `delay()`; implement non-blocking code in the superloop using `millis()` (or the platform's tick counter, e.g. `HAL_GetTick()` / `esp_timer_get_time()`).
* Create an LED module (`led.h` / `led.c`) with a `led_t` struct and the functions:
  * `void led_init(led_t *led, uint8_t pin);`
  * `void led_set(led_t *led, led_state_t state);`

### 2. Use `static const` / `enum` for parameters

* Create a configuration header (`config.h`) with a `typedef struct` holding settings, instantiated as a `static const` object, and/or an anonymous `enum` for integer constants.
* Completely eliminate "magic numbers" by moving them into named constants.
* Mark function parameters that are not modified as `const` (e.g. `const led_t *led`).

### 3. Measure superloop execution time (Optional)

* Measure the execution time of a single `loop()` iteration without `delay()` (use `micros()` or a hardware cycle counter).
* Print the average time or metrics to the Serial Monitor every 1000 iterations.
* Use `uint32_t` / `uint64_t` accumulators and fixed-size buffers for output (`snprintf` into a `char[]`); no dynamic allocation.

### 4. Add a button handled via interrupt (Optional)

* Connect a button to a GPIO pin and configure an interrupt (`attachInterrupt` or the platform equivalent).
* Use a `static volatile bool button_pressed` flag to record the press event in the ISR.
* Change the LED mode on every press using an `enum` and a state machine (`switch`): Blinking → Always On → Always Off → Blinking.
* Keep the ISR minimal: set the flag only (no `Serial.print`, no delays, no function calls that block).
* Handle all mode-switching logic and debouncing (time-based, using the tick counter) in the main loop.
* Clear the flag safely in the main loop (briefly disable interrupts or use an atomic read-and-clear) to avoid losing or double-counting events.

## Suggested Project Structure

```
project/
├── main.c        // setup + superloop
├── led.h / led.c // LED module
├── config.h      // constants and configuration struct
├── README.md     // wiring, build instructions, results
```

## Deliverables

* GitHub repository with the source code.
* README / output showing execution results (e.g., Serial Monitor output, photo or video of the working board).
* Repository link submitted in the LMS.
