#ifndef M5F_EVENTS_H
#define M5F_EVENTS_H


typedef enum dispatch_evt_type {
    GUST_EVT_BUTTON_SPEED_PRESSED,
    GUST_EVT_KNOB_CHANGED,
    GUST_EVT_WIFI_CONNECTED,
    GUST_EVT_OTA_STARTED,
    GUST_EVT_OTA_PROGRESS,
    GUST_EVT_OTA_ENDED,
    GUST_EVT_POWER_DISCONNECTED
} dispatch_evt_type_t;

typedef struct dispatch_evt {
    dispatch_evt_type_t type;
    int32_t data;
    void * extdata;
} dispatch_evt_t;

#endif