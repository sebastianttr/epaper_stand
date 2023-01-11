//
// Created by Sebastian on 27.08.22.
//

#include "HTTPEndpoint.h"
#include "Arduino.h"

#define ROUTE_SIZE 1024


void HTTPEndpoint::onGet(const char *route, response (callback)(http_request)) {
    this->addEndpoint("GET", route, callback);
}

void HTTPEndpoint::onPost(const char* route, response (callback)(http_request)) {
    this->addEndpoint("POST", route, callback);
}

void HTTPEndpoint::begin() {
    this->routes1 = {
            0,
            (char**) malloc(ROUTE_SIZE),
            (cb*) malloc(ROUTE_SIZE),
            (char**) malloc(ROUTE_SIZE)
    };
}

void HTTPEndpoint::addEndpoint(const char *method, const char *route, cb callback) {
    this->routes1.methods[this->routes1.size] = (char*)method;
    this->routes1.routes[this->routes1.size] = (char*)route;
    this->routes1.callback[this->routes1.size] = callback;
    this->routes1.size++;
}

cb HTTPEndpoint::getRouteCallback(const char* routeName, const char* method){

    for(int i = 0;i < this->routes1.size; i++){
        if(strcmp(routeName,this->routes1.routes[i]) == 0
            && strcmp(method,this->routes1.methods[i]) == 0){
            return this->routes1.callback[i];
        }
    }

    // if nothing, return nothing
    return nullptr;
}

String getRouteName(String request){
    int methodNameEnd = request.indexOf(" ") + 1;
    String requestMethodRemoved = request.substring(methodNameEnd,request.length());
    return requestMethodRemoved.substring(
            0,
            (requestMethodRemoved.indexOf("?") != -1)?requestMethodRemoved.indexOf("?"):requestMethodRemoved.indexOf(" ")
            );
}

String getMethod(String request){
    int methodNameEnd = request.indexOf(" ");
    return request.substring(0,methodNameEnd);
}


header_map getHeaders(String request){
    header_map headermap1 = {
            0,
            (char**) malloc(1024),
            (char**) malloc(4096)
    };

    int firstLineRemove = request.indexOf("\n") + 1;
    String req = request.substring(firstLineRemove,request.length());

    uint8_t counter = 0;

    while(true){
        if(req.length() <= 2 || req.indexOf(" \n") == 0){
            break;
        }

        headermap1.headerKey[counter] = (char*) malloc(512);
        headermap1.headerValue[counter] = (char*) malloc(512);

        strcpy(headermap1.headerKey[counter],req.substring(0,req.indexOf(":")).c_str());
        strcpy(headermap1.headerValue[counter],req.substring(req.indexOf(":")+2,req.indexOf("\n")-1).c_str());

        req = req.substring(req.indexOf("\n")+1,req.length());

        counter++;
    }

    headermap1.len = counter;

    return headermap1;
}

query_map getQuery(String request){
    query_map querymap1 = {
            0,
            (char**) malloc(1024),
            (char**) malloc(4096)
    };


    int methodNameEnd = request.indexOf(" ") + 1;
    String routeWithParams =
            request
                .substring(methodNameEnd, request.indexOf("\n"));


    // if there are no query params, leave
    if(routeWithParams.indexOf("?") == -1){
        return querymap1;
    }
    else {
        String routeWithParams2  = routeWithParams.substring(0,routeWithParams.indexOf(" "));

        int firstPartRemove = routeWithParams2.indexOf("?") + 1;
        String query = routeWithParams2.substring(firstPartRemove,routeWithParams2.length());


        uint8_t counter = 0;

        while(true){
            if(query.length() == 0){
                Serial.println("len is 0");
                break;
            }

            querymap1.queryName[counter] = (char*) malloc(512);
            querymap1.queryValue[counter] = (char*) malloc(512);

            strcpy(querymap1.queryName[counter],query.substring(0,query.indexOf("=")).c_str());
            strcpy(querymap1.queryValue[counter],query.substring(query.indexOf("=")+1
                    ,(query.indexOf("&") != -1)
                    ?query.indexOf("&")
                    :query.length()).c_str()
                );

            counter++;

            if(query.indexOf("&") != -1)
                query = query.substring(query.indexOf("&")+1,query.length());
            else break;
        }

        querymap1.len = counter;
    }

    return querymap1;
}



void HTTPEndpoint::handleRequest(String* request,WiFiClient* client) {

    WiFiClient clt = *client;

    String route = getRouteName(*request);
    String method = getMethod(*request);
    header_map headermap1 = getHeaders(*request);
    query_map querymap1 = getQuery(*request);


    cb callback = this->getRouteCallback(route.c_str(),method.c_str());

    if(callback == nullptr){
        clt.println("HTTP/1.1 404 Not Found");
        clt.println();

        clt.print("Route not found!");

    }
    else {

        response res = callback({
            .queryMap = querymap1,
            .headerMap = headermap1
        });


        clt.println("HTTP/1.1 " + String(res.statusCode));
        clt.printf("Content-type:%s\n",res.contentType);
        clt.println();

        // the content of the HTTP response follows the header:
        clt.print(res.content);
    }

}

const char* HTTPEndpoint::getHeaderValue(header_map* headerMap, const char *headerName) {
    for(int i = 0; i < headerMap->len; i++){
        if(strcmp(headerName, headerMap->headerKey[i]) == 0){
            return headerMap->headerValue[i];
        }
    }
    return "";
}

const char* HTTPEndpoint::getQueryValue(query_map* queryMap, const char *queryName) {
    for(int i = 0; i < queryMap->len; i++){
        if(strcmp(queryName, queryMap->queryName[i]) == 0){
            return queryMap->queryValue[i];
        }
    }
    return "";
}
