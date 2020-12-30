#include <M5StickC.h>
#include <FastLED.h>

#include "dispatch.h"
#include "leds.h"

#define PIN_LED G0

void ledsTask(void * pvParameter) {
    CRGB leds[NUM_LEDS];
    dispatch_pair_t core = *((dispatch_pair_t *) pvParameter);
    dispatch_cmd_type_t curmode = LEDS_SHOW_RAINBOW;
    // put your setup code here, to run once:
    FastLED.addLeds<NEOPIXEL, PIN_LED>(leds, NUM_LEDS);
    while (1) {
        dispatch_cmd_t curcmd;
        if (xQueueReceive(core.cmdq, &curcmd, 0) == pdTRUE) { //Non-blocking check
            curmode = curcmd.type;
        }
        else {
            if (curmode == LEDS_SHOW_RAINBOW) {
                for (int i = 0; i < (255*4); i++) {
                    fill_rainbow(leds, NUM_LEDS, i%255, 255/NUM_LEDS);
                    FastLED.show();
                    vTaskDelay(1 / portTICK_PERIOD_MS);
                }
            }
        }
    }
}