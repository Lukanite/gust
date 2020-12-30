#include <M5StickC.h>
#include "knob.h"
#include "dispatch.h"

static const char * TAG = "KNOB";

#define KNOB_PWR_PIN G33
#define KNOB_POT_PIN G32

#define KNOB_READ_BURST_SAMPLES 5 //How many times to read the ADC per knob read
#define KNOB_READ_BURST_SAMPLE_DELAY_MS 5 //How long to wait between each read
#define KNOB_READ_DELAY 75
#define KNOB_POWER_SAVE 1 //Whether to turn off the pin between reads

void knobTask(void * pvParameter) {
    uint16_t lastRead = -1; //Set to max uint so first read will send an event
    dispatch_pair_t core = *((dispatch_pair_t *) pvParameter);
    pinMode(KNOB_POT_PIN, INPUT);
    pinMode(KNOB_PWR_PIN, OUTPUT);
    digitalWrite(KNOB_PWR_PIN, HIGH);
    analogReadResolution(KNOB_ADC_BITS);
    while (1) {
        if (KNOB_POWER_SAVE != 0) digitalWrite(KNOB_PWR_PIN, HIGH);
        uint32_t sampleSum = 0;
        for (int i = 0; i < KNOB_READ_BURST_SAMPLES; i++) {
            vTaskDelay(KNOB_READ_BURST_SAMPLE_DELAY_MS/portTICK_PERIOD_MS);
            uint16_t sample = analogRead(KNOB_POT_PIN);
            ESP_LOGD(TAG, "KNOB-ADC(%i): %u", i, sample);
            sampleSum += sample;
        }
        if (KNOB_POWER_SAVE != 0) digitalWrite(KNOB_PWR_PIN, LOW);
        dispatch_evt_t evt;
        evt.type = GUST_EVT_KNOB_CHANGED;
        evt.data = sampleSum / KNOB_READ_BURST_SAMPLES;
        ESP_LOGD(TAG, "KNOB: %u", evt.data);
        if (abs(evt.data - lastRead) * 100 > (KNOB_VALUES_RANGE * KNOB_NOTIFY_CHANGE_PCT)) {
            xQueueSend(core.evtq, &evt, portMAX_DELAY);
            lastRead = evt.data;
        }
        vTaskDelay(KNOB_READ_DELAY/portTICK_PERIOD_MS);
    }
}
