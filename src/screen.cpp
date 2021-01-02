#include <M5StickC.h>
#include <WiFi.h>

#include "screen.h"
#include "dispatch.h"
#include "pulse.h"

gust_screen_t curscreen = SCREEN_NONE;
static char buf[40];
static int32_t otaFileSize, otaBytesWritten = 0;


static void showScreenOTA() {
    if (curscreen != SCREEN_OTA) {
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.drawRect(19, 29, 122, 22, TFT_WHITE);
        M5.Lcd.setTextFont(4);
        snprintf(buf, sizeof(buf), "Updating");
        M5.Lcd.setTextDatum(TC_DATUM);
        M5.Lcd.drawString(buf, 80, 0);
        curscreen = SCREEN_OTA;
    }
    int progress = otaBytesWritten * 120 / otaFileSize;
    if (progress > 120) progress = 120;
    if (progress < 0) progress = 0;
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.fillRect(20, 30, progress, 20, TFT_BLUE);
    M5.Lcd.fillRect(20 + progress, 30, 120 - progress, 20, TFT_BLACK);
    M5.Lcd.setTextFont(2);
    M5.Lcd.setTextDatum(CC_DATUM);
    snprintf(buf, sizeof(buf), "%i%%", otaBytesWritten * 100 / otaFileSize);
    M5.Lcd.drawString(buf, 80, 40);
    M5.Lcd.setTextDatum(TC_DATUM);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    snprintf(buf, sizeof(buf), "%i / %i", otaBytesWritten, otaFileSize);
    M5.Lcd.drawString(buf, 80, 60);
}

void screenTask(void * pvParameter) {
    dispatch_pair_t core = *((dispatch_pair_t *) pvParameter);
    IPAddress ip;
    M5.Lcd.begin();
    M5.Lcd.setRotation(1);
    while (1) {
        dispatch_cmd_t curcmd;
        if (xQueueReceive(core.cmdq, &curcmd, portMAX_DELAY) == pdTRUE) {
            if (curcmd.type == GUST_CMD_SCREEN_SHOW_HOME) {
                if (curscreen != SCREEN_HOME) {
                    M5.Lcd.fillScreen(TFT_BLACK);
                    M5.Lcd.setTextDatum(TL_DATUM);
                    curscreen = SCREEN_HOME;
                }
                M5.Lcd.setTextFont(4);
                snprintf(buf, sizeof(buf), "RPM: %-5i     ", getRPM());
                M5.Lcd.drawString(buf, 0, 0);
                snprintf(buf, sizeof(buf), "PWR: %-5i     ", curcmd.data);
                M5.Lcd.drawString(buf, 0, 30);
                M5.Lcd.setTextFont(2);
                snprintf(buf, sizeof(buf), "IP: %u.%u.%u.%u     ", ip[0], ip[1], ip[2], ip[3]);
                M5.Lcd.drawString(buf, 0, 60);
            }
            else if (curcmd.type == GUST_CMD_SCREEN_UPDATE_WIFI_IP) {
                ip = (uint32_t) curcmd.data;
            }
            else if (curcmd.type == GUST_CMD_SCREEN_SHOW_POWEROFF) {
                if (curscreen != SCREEN_POWEROFF) {
                    M5.Lcd.fillScreen(TFT_BLACK);
                    M5.Lcd.setTextDatum(TL_DATUM);
                    curscreen = SCREEN_POWEROFF;
                }
                M5.Lcd.setTextFont(4);
                snprintf(buf, sizeof(buf), "Power off");
                M5.Lcd.drawString(buf, 0, 0);
            }
            else if (curcmd.type == GUST_CMD_SCREEN_SHOW_OTA) {
                otaFileSize = curcmd.data;
                otaBytesWritten = 0;
                showScreenOTA();
            }
            else if (curcmd.type == GUST_CMD_SCREEN_UPDATE_OTA_PROGRESS) {
                otaBytesWritten = curcmd.data;
                showScreenOTA();
            }
        }
    }
}