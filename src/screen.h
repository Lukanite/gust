#ifndef M5F_SCREEN_H
#define M5F_SCREEN_H

typedef enum gust_screen {
    SCREEN_NONE,
    SCREEN_HOME,
    SCREEN_OTA,
    SCREEN_POWEROFF
} gust_screen_t;

void screenTask(void * pvParameter);

#endif