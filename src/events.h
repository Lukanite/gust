#ifndef M5F_EVENTS_H
#define M5F_EVENTS_H


typedef enum dispatch_evt_type {
    GUST_EVT_BUTTON_SPEED_PRESSED,
} dispatch_evt_type_t;

typedef struct dispatch_evt {
    dispatch_evt_type_t type;
    int data;
    void * extdata;
} dispatch_evt_t;

#endif