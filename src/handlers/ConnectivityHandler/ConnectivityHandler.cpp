//
// Created by Sebastian on 01.12.22.
//

#include "ConnectivityHandler.h"

void ConnectivityHandler::initEndpointHandler() {

    this->httpEndpoint.onGet("/hello", [](http_request httpRequest) -> response {
        return {
                .statusCode = 200,
                .contentType = "text/plain",
                .content = "Hello World!!!"
        };
    });

    /*
     * TODO: Add set location
     * TODO: Email Access Token -> for accesing the emails from the device itself -> could need a dedicated server for it
     * TODO: Add calender schedule
     * TODO: Remove calender schedule
     * TODO: Change calender schedule
     */

    this-> httpEndpoint.onGet("/setLocation", [](http_request httpRequest) -> response {
        return {
                .statusCode = 200,
                .contentType = "text/plain",
                .content = "Setting the location!"
        };
    });

    this-> httpEndpoint.onGet("/addCalenderSchedule", [](http_request httpRequest) -> response {
        return {
                .statusCode = 200,
                .contentType = "text/plain",
                .content = "Adding calender schedule"
        };
    });

    this-> httpEndpoint.onGet("/removeCalenderSchedule", [](http_request httpRequest) -> response {
        return {
                .statusCode = 200,
                .contentType = "text/plain",
                .content = "Remove Calender Schedule"
        };
    });

    this-> httpEndpoint.onGet("/changeCalenderSchedule", [](http_request httpRequest) -> response {
        return {
                .statusCode = 200,
                .contentType = "text/plain",
                .content = "Change Calender Schedule"
        };
    });


}


void ConnectivityHandler::tryWiFiConnect() {

    WiFi.begin(ssid, password);
    Serial.println("Connecting ... ");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println(WiFi.status());


    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

}

void ConnectivityHandler::initNTP() {
    ntp.ruleDST("CEST", Last, Sun, Mar, 2, 120); // last sunday in march 2:00, timetone +120min (+1 GMT + 1h summertime offset)
    ntp.ruleSTD("CET", Last, Sun, Oct, 3, 60);
    ntp.begin();
    ntp.update();
}

void ConnectivityHandler::handleHTTPServer() {
    WiFiClient client = wiFiServer.available();

    char lastChar;

    if (client) {
        String httpRequest = "";
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                if (c == '\n' && lastChar == '\n') {
                    break;
                } else if (c != '\r') {
                    httpRequest += c;
                    lastChar = c;
                }
            }
        }

        httpEndpoint.handleRequest(&httpRequest,&client);

        // close the connection:
        client.stop();
    }
}

void ConnectivityHandler::begin() {
    this->tryWiFiConnect();
    this->wiFiServer.begin();
    this->httpEndpoint.begin();
    this->initEndpointHandler();
    this->initNTP();
}

void ConnectivityHandler::handle() {
    this->ntp.update();
    this->handleHTTPServer();
}
