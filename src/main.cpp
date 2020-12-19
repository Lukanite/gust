#include <Arduino.h>
#include <M5StickC.h>

#include "driver\pcnt.h"
#include "dispatch.h"

//Tasks
#include "screen.h"
#include "fan.h"
#include "leds.h"
#include "pulse.h"

QueueHandle_t cevtQueue;

dispatch_pair_t dispatchScreen;
dispatch_pair_t dispatchFan;
dispatch_pair_t dispatchLeds;

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

  xTaskCreate(screenTask, "Screen", 4*1024, &dispatchScreen, 10, NULL);
  xTaskCreate(fanTask, "Fan PWM", 4*1024, &dispatchFan, 10, NULL);
  xTaskCreate(ledsTask, "LEDs", 4*1024, &dispatchLeds, 10, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int h = 0; h <= 10; h++) {
    for (int i = 0; i < 10; i++) {
      dispatch_cmd_t cmd;
      cmd.type = SCREEN_SHOW_HOME;
      xQueueSend(dispatchScreen.cmdq, &cmd, portMAX_DELAY);
      vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    dispatch_cmd_t cmdb;
    cmdb.type = FAN_SET_PERCENT;
    cmdb.data = h * 10;
    xQueueSend(dispatchFan.cmdq, &cmdb, portMAX_DELAY);
  }
}