#include <Arduino.h>
#include <M5StickC.h>

#include "driver\pcnt.h"
#include "dispatch.h"

//Tasks
#include "screen.h"
#include "fan.h"
#include "leds.h"
#include "pulse.h"
#include "buttons.h"
#include "knob.h"

QueueHandle_t cevtQueue;

dispatch_pair_t dispatchScreen;
dispatch_pair_t dispatchFan;
dispatch_pair_t dispatchLeds;
dispatch_pair_t dispatchButtons;
dispatch_pair_t dispatchKnob;

int curpwr = 0;

void setup() {
  M5.begin();
  initializePulseCounter();
  cevtQueue = xQueueCreate(20, sizeof(dispatch_evt_t));
  dispatchScreen.evtq = cevtQueue;
  dispatchScreen.cmdq = xQueueCreate(20, sizeof(dispatch_cmd_t));
  dispatchFan.evtq = cevtQueue;
  dispatchFan.cmdq = xQueueCreate(20, sizeof(dispatch_cmd_t));
  dispatchLeds.evtq = cevtQueue;
  dispatchLeds.cmdq = xQueueCreate(20, sizeof(dispatch_cmd_t));
  dispatchButtons.evtq = cevtQueue;
  dispatchButtons.cmdq = xQueueCreate(20, sizeof(dispatch_cmd_t)); //Not used
  dispatchKnob.evtq = cevtQueue;
  dispatchKnob.cmdq = xQueueCreate(20, sizeof(dispatch_cmd_t)); //Not used

  xTaskCreate(screenTask, "Screen", 4*1024, &dispatchScreen, 10, NULL);
  xTaskCreatePinnedToCore(fanTask, "Fan PWM", 4*1024, &dispatchFan, 10, NULL, 1);
  xTaskCreate(ledsTask, "LEDs", 4*1024, &dispatchLeds, 10, NULL);
  xTaskCreate(buttonTask, "Buttons", 4*1024, &dispatchButtons, 10, NULL);
  xTaskCreatePinnedToCore(knobTask, "Knob", 4*1024, &dispatchKnob, 1, NULL, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (1) {
    dispatch_evt_t curevt;
    if (xQueueReceive(cevtQueue, &curevt, 100/portTICK_PERIOD_MS) == pdTRUE) {
      if (curevt.type == GUST_EVT_BUTTON_SPEED_PRESSED) {
        curpwr += 10;
        if (curpwr > 100) curpwr = 0;
        dispatch_cmd_t cmdb;
        cmdb.type = FAN_SET_PERCENT;
        cmdb.data = curpwr;
        xQueueSend(dispatchFan.cmdq, &cmdb, portMAX_DELAY);
      }
      else if (curevt.type == GUST_EVT_KNOB_CHANGED) {
        dispatch_cmd_t cmdb;
        curpwr = (curevt.data + 1) * 100 / KNOB_VALUES_RANGE; //Add 1 so 100% can be hit
        if (curpwr > 100 - (2 * KNOB_NOTIFY_CHANGE_PCT)) curpwr = 100; //Set upper threshold for pot
        if (curpwr < (2 * KNOB_NOTIFY_CHANGE_PCT)) curpwr = 0; //Set lower theshold for pot
        cmdb.type = FAN_SET_PERCENT;
        cmdb.data = curpwr;
        xQueueSend(dispatchFan.cmdq, &cmdb, portMAX_DELAY);
      }
    } 
    dispatch_cmd_t cmd;
    cmd.type = SCREEN_SHOW_HOME;
    cmd.data = curpwr;
    xQueueSend(dispatchScreen.cmdq, &cmd, portMAX_DELAY);
  }
}