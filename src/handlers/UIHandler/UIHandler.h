//
// Created by Sebastian on 02.12.22.
//

#ifndef EPAPER_FIRMWARE_UIHANDLER_H
#define EPAPER_FIRMWARE_UIHANDLER_H
#include "types/typedefs.h"
#include "lvgl.h"

class UIHandler {

public:

    UIHandler();
    void init();
    lv_obj_t* getImage(const char* val);

    lv_obj_t drawFirstScreen(
            weather_data* weatherData,
            int8_t hour,
            int8_t minute,
            int8_t day,
            int8_t month,
            int16_t year
    );

    lv_obj_t drawFirstScreenTest(
            weather_data* weatherData,
            int8_t hour,
            int8_t minute,
            int8_t day,
            int8_t month,
            int16_t year
    );

    lv_obj_t drawCalenderScreen();

    lv_obj_t* imgSnow;
    lv_obj_t* imgClouds;
    lv_obj_t* imgSun;
};


#endif EPAPER_FIRMWARE_UIHANDLER_H
