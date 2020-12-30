#include <M5StickC.h>
#include "fan.h"
#include "dispatch.h"

#define LEDC_CHANNEL 0
#define PWM_FREQ 25000
#define PWM_BITS 8
#define PWM_MAX (1 << PWM_BITS)
#define PIN_PWM G26

static const char * TAG = "FAN";

void fanTask(void * pvParameter) {
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
    dispatch_pair_t core = *((dispatch_pair_t *) pvParameter);
    ledcSetup(LEDC_CHANNEL, PWM_FREQ, PWM_BITS);
    ledcAttachPin(PIN_PWM, LEDC_CHANNEL);
    while (1) {
        dispatch_cmd_t curcmd;
        if (xQueueReceive(core.cmdq, &curcmd, portMAX_DELAY) == pdTRUE) {
            if (curcmd.type == FAN_SET_PERCENT) {
                uint32_t duty = (curcmd.data * (PWM_MAX - 1)) / 100;
                ESP_LOGD(TAG, "Duty is now %u", duty);
                ledcWrite(LEDC_CHANNEL, duty);
            }
        }
    }
}
