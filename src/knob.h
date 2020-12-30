#ifndef M5F_KNOB_H
#define M5F_KNOB_H

#define KNOB_ADC_BITS 9 //We don't need much precision here (9-12)
#define KNOB_VALUES_RANGE (1 << KNOB_ADC_BITS)
#define KNOB_NOTIFY_CHANGE_PCT 1 //How much the knob has to change to generate an event

void knobTask(void * pvParameter);

#endif