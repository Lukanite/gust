#include "pulse.h"

static const char * TAG = "PULSE";

pulse_info_t PulseInfo;

static void IRAM_ATTR pulseISR() {
    int64_t newtime = esp_timer_get_time();
    PulseInfo.latestPulseDuration = newtime - PulseInfo.latestPulseTime;
    PulseInfo.latestPulseTime = newtime;
}

void initializePulseCounter() {
    pinMode(PULSE_GPIO, INPUT); 
    gpio_pulldown_dis(GPIO_NUM_25); //Disable pullups due to M5StickC limitation between G25/G36
    gpio_pullup_dis(GPIO_NUM_25);
    attachInterrupt(PULSE_GPIO, pulseISR, RISING);
}

//Gets RPM of fan
int16_t getRPM() {
    if (esp_timer_get_time() - PulseInfo.latestPulseTime > PULSE_ZERO_THRESHOLD) return 0;
    return (int16_t)(((uint64_t)1000 * 1000 * 30) / (PulseInfo.latestPulseDuration)); //Convert to RPM (2 pulses/rev)
}