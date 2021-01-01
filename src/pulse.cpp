#include "pulse.h"

#include "driver\pcnt.h"

#define PULSE_MEASUREMENT_COUNT 10

static const char * TAG = "PULSE";

volatile pulse_info_t PulseInfo;

static void IRAM_ATTR pcnt_intr_handler(void *arg) //Only called when limit is reached
{
    pulse_info_t * pulseInfo = (pulse_info_t *) arg;
    pulseInfo->earlierPulseTime = pulseInfo->lastPulseTime;
    pulseInfo->lastPulseTime = esp_timer_get_time();
    ESP_ERROR_CHECK(pcnt_counter_clear(PULSE_UNIT));
}

void initializePulseCounter() {
    pcnt_config_t config = {};
    config.pulse_gpio_num = PULSE_GPIO;
    config.ctrl_gpio_num = PCNT_PIN_NOT_USED;
    config.channel = PCNT_CHANNEL_0;
    config.unit = PULSE_UNIT;
    config.neg_mode = PCNT_COUNT_INC;
    config.pos_mode = PCNT_COUNT_DIS;
    config.counter_h_lim = PULSE_MEASUREMENT_COUNT;
    pcnt_unit_config(&config);
    pcnt_set_filter_value(PULSE_UNIT, 1023); //Apply the slowest filter possible to attempt to filter out glitches (still passes like in the khz range)
    pcnt_filter_enable(PULSE_UNIT);
    pcnt_event_enable(PULSE_UNIT, PCNT_EVT_H_LIM);
    pcnt_isr_service_install(0);
    pcnt_isr_handler_add(PULSE_UNIT, pcnt_intr_handler, (void *) &PulseInfo);
    
    //For some reason, this needs to be called, else it doesn't start
    pcnt_counter_pause(PULSE_UNIT);
    pcnt_counter_clear(PULSE_UNIT);
    pcnt_counter_resume(PULSE_UNIT);
}

//Gets RPM of fan
int16_t getRPM() {
    int64_t curtime = esp_timer_get_time();
    if (curtime - PulseInfo.lastPulseTime > PULSE_ZERO_THRESHOLD) return 0;
    return (int16_t)(((uint64_t)(PULSE_MEASUREMENT_COUNT) * 1000 * 1000 * 30) / (PulseInfo.lastPulseTime - PulseInfo.earlierPulseTime)); //Convert to RPM
}