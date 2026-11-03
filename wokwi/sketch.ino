// Lesson 2.3: Embedded C basics - Beetroot Academy (ESP32-S3, ESP-IDF, C)
// Source: https://github.com/eoffers51/Lesson_2_3
//
// Wokwi compiles this project with the Arduino ESP32 core, which has its own app_main().
// So main.c here renames ours to lesson_2_3_app_main() and setup() starts it.
// Everything else is the repo's ESP-IDF code.

extern "C" void lesson_2_3_app_main(void);

void setup()
{
    lesson_2_3_app_main(); // never returns: the superloop runs here
}

void loop() {}
