#include <M5StickC.h>
#include <WiFi.h>

#include "screen.h"
#include "dispatch.h"
#include "pulse.h"


void screenTask(void * pvParameter) {
    dispatch_pair_t core = *((dispatch_pair_t *) pvParameter);
    char buf[40];
    IPAddress ip;
    M5.Lcd.begin();
    M5.Lcd.setRotation(1);
    while (1) {
        dispatch_cmd_t curcmd;
        if (xQueueReceive(core.cmdq, &curcmd, portMAX_DELAY) == pdTRUE) {
            if (curcmd.type == SCREEN_SHOW_HOME) {
                M5.Lcd.setTextFont(4);
                snprintf(buf, sizeof(buf), "RPM: %-5i     ", getRPM());
                M5.Lcd.drawString(buf, 0, 0);
                snprintf(buf, sizeof(buf), "PWR: %-5i     ", curcmd.data);
                M5.Lcd.drawString(buf, 0, 30);
                M5.Lcd.setTextFont(2);
                snprintf(buf, sizeof(buf), "IP: %u.%u.%u.%u     ", ip[0], ip[1], ip[2], ip[3]);
                M5.Lcd.drawString(buf, 0, 60);
            }
            else if (curcmd.type == SCREEN_UPDATE_WIFI_IP) {
                M5.Lcd.setTextFont(1);
                ip = (uint32_t) curcmd.data;
            }
        }
    }
}