//
// Created by Sebastian on 02.12.22.
//

#ifndef EPAPER_FIRMWARE_HTTPCLIENT_H
#define EPAPER_FIRMWARE_HTTPCLIENT_H

#include "WString.h"

typedef void (*callback_t)(String);

enum HTTP_METHOD {
    HTTP_GET,
    HTTP_POST
};

typedef struct{
    HTTP_METHOD method;
    String url;
    callback_t callback;
}task_param;

void fetchAsync(HTTP_METHOD method, const String& url,callback_t callback);

#endif //EPAPER_FIRMWARE_HTTPCLIENT_H