//
// Created by Sebastian on 02.12.22.
//

#ifndef EPAPER_FIRMWARE_TYPEDEFS_H
#define EPAPER_FIRMWARE_TYPEDEFS_H

typedef struct {
    const char* date;
    float temp;
    float lowest;
    float highest;
    const char* weather;
    const char* description;
}weather_data;

typedef struct{
    int hour;
    int minute;
    int day;
    int month;
    int year;
    int weekday;
}datetime_t;

typedef struct{
    const char* name;
    datetime_t start;
    datetime_t end;
}schedule_t;

typedef struct{
    const char* title;
    const char* description;
    const char* date;
}news_item_t;

#endif EPAPER_FIRMWARE_TYPEDEFS_H
