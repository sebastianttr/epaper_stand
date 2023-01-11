//
// Created by Sebastian on 02.12.22.
//

#ifndef EPAPER_FIRMWARE_UIHANDLER_H
#define EPAPER_FIRMWARE_UIHANDLER_H
#include "types/typedefs.h"

class UIHandler {
public:

    static void drawFirstScreen(
            weather_data* weatherData,
            int8_t hour,
            int8_t minute,
            int8_t day,
            int8_t month,
            int16_t year
    );

    static void drawCalenderScreen();
};


#endif EPAPER_FIRMWARE_UIHANDLER_H
