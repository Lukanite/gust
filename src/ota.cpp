#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

#include "dispatch.h"
#include "ota.h"

//Wifi creds go into this file - see wifi_placeholder.h
#include "config.h"

WebServer server(80);

const char * headerKeys[] = {"X-Firmware-Size"};
const size_t numberOfHeaders = 1;
 
/*
 * Server Index Page
 */
 
const char* serverIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update' id='fileinput'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var size = $('#fileinput')[0].files[0].size;"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "beforeSend: function(request){request.setRequestHeader('X-Firmware-Size', size);},"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')" 
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";

/*
 * setup function
 */
void otaTask(void * pvParameter) {
    dispatch_pair_t core = *((dispatch_pair_t *) pvParameter);
    int32_t byteswritten = 0;
    // Connect to WiFi network
    WiFi.begin(GUST_WIFI_SSID, GUST_WIFI_PASS);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(500/portTICK_PERIOD_MS);
    }

    dispatch_evt_t evt;
    evt.type = GUST_EVT_WIFI_CONNECTED;
    evt.data = WiFi.localIP();
    xQueueSend(core.evtq, &evt, portMAX_DELAY);

    /*attempt to use mdns for host name resolution*/
    if (!MDNS.begin(GUST_WIFI_HOST)) { //http://gust.local
        //Ignore this error
    }

    server.collectHeaders(headerKeys, numberOfHeaders);

    /*return index page which is stored in serverIndex */
    server.on("/", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", serverIndex);
    });
    /*handling uploading firmware file */
    server.on("/update", HTTP_POST, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
    }, [&core, &byteswritten]() {
        HTTPUpload& upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
            Serial.printf("Update: %s\n", upload.filename.c_str());
            byteswritten = 0;
            dispatch_evt_t outevt;
            outevt.type = GUST_EVT_OTA_STARTED;
            outevt.data = server.header("X-Firmware-Size").toInt();
            Serial.println(server.header("X-Firmware-Size"));
            Serial.printf("Size: %i\n", outevt.data);
            xQueueSend(core.evtq, &outevt, portMAX_DELAY);
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            byteswritten += upload.currentSize;
            dispatch_evt_t outevt;
            outevt.type = GUST_EVT_OTA_PROGRESS;
            outevt.data = byteswritten;
            xQueueSend(core.evtq, &outevt, 0); //Non-blocking; don't hold up the update if something has gone wrong
            /* flashing firmware to ESP*/
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) { //true to set the size to the current progress
                Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
                dispatch_evt_t outevt;
                outevt.type = GUST_EVT_OTA_ENDED;
                xQueueSend(core.evtq, &outevt, portMAX_DELAY);
            } else {
                Update.printError(Serial);
            }
        }
    });
    server.begin();
    while (1) {
        server.handleClient();
        vTaskDelay(1/portTICK_PERIOD_MS);
    }
}
