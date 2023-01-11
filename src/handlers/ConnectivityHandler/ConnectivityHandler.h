//
// Created by Sebastian on 01.12.22.
//

#ifndef EPAPER_FIRMWARE_CONNECTIVITYHANDLER_H
#define EPAPER_FIRMWARE_CONNECTIVITYHANDLER_H


#include "http/HTTPEndpoint/HTTPEndpoint.h"
#include "NTP.h"

class ConnectivityHandler {
public:
    const char* ssid = "Speedy Gonzales";
    const char* password = "tenerife";
    const uint8_t port = 80;

    HTTPEndpoint httpEndpoint;
    WiFiServer wiFiServer = WiFiServer(port);
    WiFiUDP wiFiUdp;
    NTP ntp = NTP(wiFiUdp);

    void begin();
    void handle();

private:
    void tryWiFiConnect();
    void initEndpointHandler();
    void initNTP();
    void handleHTTPServer();
};


#endif EPAPER_FIRMWARE_CONNECTIVITYHANDLER_H
