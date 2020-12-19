#include "pulse.h"

#include "driver\pcnt.h"

static const char * TAG = "PULSE";

pulse_info_t PulseInfo;

void initializePulseCounter() {
    //Todo: Set interrupt on first count to set time of first pulse
    pcnt_config_t config = {};
    config.pulse_gpio_num = PULSE_GPIO;
    config.ctrl_gpio_num = PCNT_PIN_NOT_USED;
    config.channel = PCNT_CHANNEL_0;
    config.unit = PULSE_UNIT;
    config.neg_mode = PCNT_COUNT_INC;
    config.pos_mode = PCNT_COUNT_DIS;
    pcnt_unit_config(&config);
    
    //For some reason, this needs to be called, else it doesn't start
    pcnt_counter_pause(PULSE_UNIT);
    pcnt_counter_clear(PULSE_UNIT);
    pcnt_counter_resume(PULSE_UNIT);
}

//Gets the pulse count 
static int16_t getPulseCount() {
    int16_t newval;
    ESP_ERROR_CHECK(pcnt_get_counter_value(PULSE_UNIT, (int16_t *) &newval));
    return newval;
}

//Gets RPM of fan
int16_t getRPM() {
    int16_t newPulseCount = getPulseCount();
    uint16_t newPulses = newPulseCount - PulseInfo.lastPulseCount;
    int64_t pulseTime = esp_timer_get_time();
    int64_t duration = pulseTime - PulseInfo.lastPulseTime;
    PulseInfo.lastPulseCount = newPulseCount;
    PulseInfo.lastPulseTime = pulseTime;
    PulseInfo.debugNewPulses = newPulses;
    PulseInfo.debugDuration = duration;
    return (int16_t)(((uint64_t)(newPulses) * 1000 * 1000 * 60) / duration); //Convert to RPM
}