//
// Created by Sebastian on 01.12.22.
//
#include "HttpClient.h"
#include <WiFiClient.h>
#include <HTTPClient.h>

WiFiClient client;

void IRAM_ATTR makeRequest(void *pvParameter){
    task_param* taskParam = (task_param*)pvParameter;

    //Serial.println(taskParam->url);

    HTTPClient http;
    http.begin(client,taskParam->url);
    if(taskParam->method == HTTP_GET){
        //Serial.println("Is a get request");
        //Serial.println(taskParam->url);
        int code = http.GET();
        //Serial.println("Get successful.");

        String res = "";

        if (code == 200) {
            res = http.getString();
            Serial.printf("[HTTPS] GET... code: %d\n", code);

        }
        else {
            Serial.printf("[HTTPS] GET... failed, error: %s\n\r", String(code).c_str());
        }
        http.end();

        taskParam->callback(res);
    }
    vTaskDelete(nullptr);
}

String fetch(HTTP_METHOD method, const String& url){
    HTTPClient http;
    http.begin(client,url);
    if(method == HTTP_GET){
        int code = http.GET();
        String res = "";

        if (code == 200) {
            //Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
            res = http.getString();
        }
        else {
            Serial.printf("[HTTPS] GET... failed, error: %s\n\r", String(code).c_str());
        }
        http.end();


        return res;
    }
    return "";
}

void fetchAsync(HTTP_METHOD method, const String& url,callback_t callback) {

    static task_param taskParam = {};
    taskParam.url = url;
    taskParam.callback = callback;
    taskParam.method = method;

    xTaskCreate(makeRequest,
                "httpTask",
                4096,
                (void*)&taskParam,
                0,
                nullptr);

}

