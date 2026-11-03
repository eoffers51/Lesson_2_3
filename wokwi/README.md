# Wokwi-only files

The [Wokwi project](https://wokwi.com/projects/479662442477854721) holds a copy of this repo's code so it runs in the browser. Wokwi compiles ESP-IDF code with the **Arduino ESP32 core**, which needs two small changes:

| File in Wokwi | Comes from |
|---|---|
| `sketch.ino` | [`wokwi/sketch.ino`](sketch.ino): `setup()` calls our `app_main`, renamed to `lesson_2_3_app_main` (the Arduino core already has an `app_main`) |
| `main.c` | `main/main.c` with `#define app_main lesson_2_3_app_main` added as the first line |
| `led.c` | [`wokwi/led.c`](led.c): same `led.h` API, using the core's `rgbLedWrite()` instead of the `espressif/led_strip` component (not available on Wokwi) |
| `led.h`, `button.c`, `button.h`, `config.h` | copied unchanged from `main/` |
| `diagram.json` | [`diagram.json`](../diagram.json) |

When the code changes, upload the changed files to the Wokwi project again (file menu ▾ → *Upload file(s)…*) and press **Save**.
