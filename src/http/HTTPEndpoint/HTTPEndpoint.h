//
// Created by Sebastian on 27.08.22.
//

#ifndef HTTPENDPOINT_H
#define HTTPENDPOINT_H

#include <cstddef>
#include "map"
#include "Arduino.h"
#include "WiFi.h"

typedef struct {
    uint8_t len;
    char** headerKey;
    char** headerValue;
} header_map;

typedef struct {
    uint8_t len;
    char** queryName;
    char** queryValue;
} query_map;

typedef struct {
    uint8_t statusCode;
    const char* contentType;
    const char* content;
} response;

typedef struct {
    query_map queryMap;
    header_map headerMap;
} http_request;

typedef response (*cb)(http_request);

typedef struct{
    int size;
    char** methods;
    cb* callback;
    char** routes;
} routes;

class HTTPEndpoint {
    public:
        void begin();
        void onGet(const char* route, response (callback)(http_request));
        void onPost(const char* route, response (callback)(http_request));
        void handleRequest(String* request,WiFiClient* client);
        static const char* getHeaderValue(header_map* headerMap, const char* headerName);
        static const char* getQueryValue(query_map* queryMap, const char* queryName);
    private:
        WiFiServer wiFiServer;
        routes routes1;
        void addEndpoint(const char* method, const char* route, cb callback);
        cb getRouteCallback(const char* routeName, const char* method);
};


#endif HTTPENDPOINT_H
