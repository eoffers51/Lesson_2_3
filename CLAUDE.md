# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

ESP-IDF v6.0.2 project for an **ESP32-S3-DevKitC-1** clone (Beetroot Academy lesson 2.3). The requirements are in `TASK.md`: Embedded C blink with an LED module, a config header, optional superloop timing and an optional interrupt-driven button. Check changes against it.

## Build / flash

`idf.py` is not on PATH. Activate the IDF env in the same PowerShell call as every `idf.py` command (shell state doesn't persist between calls):

```powershell
. C:\Espressif\tools\Microsoft.v6.0.2.PowerShell_profile.ps1 *> $null; idf.py build
```

- Activating the env also puts Espressif's `cmake`/`ninja` ahead of the STM32CubeCLT ones on PATH, so never build without it.
- `sdkconfig` is gitignored and may not exist. If it's missing, run `idf.py set-target esp32s3` before the first build.
- Flash with `idf.py -p COMx flash` (or use `/build-flash`). Never run `idf.py monitor`: it's interactive and will hang, so ask the user to run it.

## Layout

Keep the modules flat in `main/` as `TASK.md` suggests: `main.c` (thin `app_main` that sets up and runs the superloop), `led.h`/`led.c` and `config.h`. Add every new `.c` to `SRCS` in `main/CMakeLists.txt`. Registry dependencies (e.g. `espressif/led_strip`) go in `main/idf_component.yml` and add `PRIV_REQUIRES` for them.

## Task rules (graded, easy to violate)

- Plain **C11**, with no C++ features.
- `#define` **only for include guards**. Pins, intervals and other numbers go in `static const` / anonymous `enum` / a `static const` config struct in `config.h`. No magic numbers.
- No global variables: state is `static` at file scope and reached through functions. No `malloc`/`free`.
- Non-blocking superloop: time with `esp_timer_get_time()`, no `vTaskDelay`-based blinking. The loop may still need a short yield (e.g. `vTaskDelay(1)`) so the idle task's watchdog doesn't fire.
- Parameters that aren't modified are `const` (e.g. `const led_t *led`).
- Button ISR only sets a `static volatile bool` flag. Debounce, mode switching (Blinking → Always On → Always Off → Blinking, `switch` on an `enum`) and atomic flag clearing all happen in the main loop.
- Output via `snprintf` into fixed `char[]` buffers, logged with `ESP_LOGI` and a `TAG`.

## Hardware

- LED = the onboard **WS2812 RGB LED on GPIO48** (confirmed on this board; GPIO38 does nothing). It is addressable, so `gpio_set_level` won't light it. Drive it with `espressif/led_strip` (RMT backend, 1 LED) behind the required `led_init(led_t *led, uint8_t pin)` / `led_set(led_t *led, led_state_t state)` API.
- Button = onboard **Boot button, GPIO0, active LOW** (use the internal pull-up and a falling-edge interrupt via `gpio_install_isr_service` + `gpio_isr_handler_add`). The ISR must be `IRAM_ATTR`.
- The wiring is drawn in `docs/wiring.svg` (README picture) and `diagram.json` (Wokwi, with `wokwi.toml`; Wokwi-only glue in `wokwi/`). If a pin or the code changes, update them and re-upload to the Wokwi project.
- Serial port is COM5 (CH343 USB-UART). The small green LED that flickers with log output is the UART TX light, not the app's LED.

## Code style

Formatting follows `.clang-format` (ESP-IDF style: 4 spaces, function braces on their own line). A hook applies it after edits.
