//
// Created by Sebastian on 02.12.22.
//

#include <WString.h>
#include <Arduino.h>
#include "UIHandler.h"
#include "misc/lv_style.h"
#include "core/lv_obj.h"
#include "core/lv_disp.h"
#include "widgets/lv_label.h"
#include "widgets/lv_line.h"

#include "images/image_clouds.h"
#include "images/image_sunny.h"
#include "images/image_snow.h"
#include "widgets/lv_img.h"

const schedule_t schedule[] = {
        {
                "Homework 1",
                {8,15,1,1,2023,0},
                {10,15,1,1,2023, 0}
        },
        {
                "Homework 2",
                {10,15,2,1,2023,1},
                {12,15,2,1,2023,1}
        },
        {
                "Homework 3",
                {8,15,3,1,2023,2},
                {11,15,3,1,2023,2}
        },
        {
                "Homework 4",
                {11,30,4,1,2023,3},
                {14,15,4,1,2023,3}
        },
        {
                "Homework 5",
                {10,45,5,1,2023,4},
                {18,15,5,1,2023,4}
        }
};

void setSource(const char* val, lv_obj_t * img1){
    if(strcmp(val,"Snow") == 0)
        lv_img_set_src(img1, &snow);
    else if(strcmp(val,"Clouds") == 0)
        lv_img_set_src(img1, &clouds);
    else lv_img_set_src(img1, &day_sunny_icon);
}

lv_obj_t* UIHandler::getImage(const char* val){
    if(strcmp(val,"Snow") == 0)
        return imgSun;
    else if(strcmp(val,"Clouds") == 0)
        return imgClouds;
    else return imgSnow;
}

const char* leadingZero(int num){
    char* text = (char*) malloc(64);
    if(num < 10)
        sprintf(text,"%d%d",0,num);
    else
        sprintf(text,"%d",num);

    return text;
}

const char* composeTime(uint8_t hour,uint8_t minute){
    char* time = (char*) malloc(64);
    sprintf(time,"%s:%s", leadingZero(hour), leadingZero(minute));
    return time;
}

char* compose_time_frame(const datetime_t* datetime_1,const datetime_t* datetime_2){
    char *ret =  (char*)malloc(sizeof(char*) + 4096);

    sprintf(
            ret,
            "%s:%s - %s:%s",
            leadingZero(datetime_1->hour),
            leadingZero(datetime_1->minute),
            leadingZero(datetime_2->hour),
            leadingZero(datetime_2->minute)
    );

    return ret;
}

int time_to_pos(const datetime_t* datetime){
    const int containerSize = 366;
    const int hoursVisible = 16;
    int pos = 0;

    pos = (datetime->hour - 6) * (containerSize / hoursVisible);
    pos += (containerSize/hoursVisible) * (datetime->minute/60);

    return pos;
}

const char* composeDate(uint8_t day,uint8_t month,uint8_t year){
    char* date = (char*) malloc(64);
    sprintf(date,"%s / %s / %s ", leadingZero(month), leadingZero(day), leadingZero(2022));
    return date;
}

lv_obj_t UIHandler::drawCalenderScreen(){
    static lv_style_t style_top;
    lv_style_init(&style_top);
    lv_style_set_text_font(&style_top, &lv_font_montserrat_26);

    lv_obj_t *label_time = lv_label_create( lv_scr_act() );
    lv_obj_add_style(label_time,&style_top,0);
    lv_label_set_text( label_time, "09:40" );
    lv_obj_set_align(label_time, LV_TEXT_ALIGN_RIGHT);
    lv_obj_align( label_time, LV_ALIGN_TOP_LEFT, 20, 10 );

    lv_obj_t *label_date = lv_label_create( lv_scr_act() );
    lv_obj_add_style(label_date,&style_top,0);
    lv_label_set_text( label_date, "Mittwoch, 30.November 2022" );
    lv_obj_set_align(label_date, LV_TEXT_ALIGN_LEFT);
    lv_obj_align( label_date, LV_ALIGN_TOP_RIGHT, -20, 10 );

    static lv_style_t calender_container_style;
    lv_style_init(&calender_container_style);
    lv_style_set_border_color(&calender_container_style, lv_color_black());

    lv_style_set_pad_all(&calender_container_style, 0);

    lv_obj_t * calender_container;
    calender_container = lv_obj_create(lv_scr_act());
    lv_obj_set_height(calender_container, 420);
    lv_obj_set_width(calender_container, 760);
    lv_obj_add_style(calender_container, &calender_container_style, 0);
    lv_obj_align(calender_container, LV_ALIGN_CENTER, 0, 20);

    /*Create an array for the points of the line*/
    static lv_point_t line_points[] = { {30, 54}, {750, 54} };

    /*Create style*/
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 2);
    lv_style_set_line_color(&style_line, lv_color_make(0,0,0));
    lv_style_set_line_rounded(&style_line, true);

    /*Create a line and apply the new style*/
    lv_obj_t * line1;
    line1 = lv_line_create(calender_container);
    lv_line_set_points(line1, line_points, 2);     /*Set the points*/
    lv_obj_add_style(line1, &style_line, 0);


    const int days_shown = 7;

    for(int i = 0; i < days_shown;i++){
        static lv_style_t style_dates;
        lv_style_init(&style_dates);

        lv_style_set_text_font(&style_dates, &lv_font_montserrat_16);

        lv_obj_t *label_schedule_day = lv_label_create( calender_container);
        lv_obj_add_style(label_schedule_day,&style_dates,0);
        lv_obj_set_width(label_schedule_day, 95);
        lv_label_set_text( label_schedule_day, "Friday");
        lv_label_set_long_mode(label_schedule_day, LV_LABEL_LONG_CLIP);
        lv_obj_align( label_schedule_day, LV_ALIGN_TOP_LEFT, i*(740 / 7) + 30 , 10);

        lv_obj_t *label_schedule_date = lv_label_create( calender_container);
        lv_obj_add_style(label_schedule_date,&style_dates,0);
        lv_obj_set_width(label_schedule_date, 95);
        lv_obj_set_align(label_schedule_date, LV_TEXT_ALIGN_RIGHT);
        lv_label_set_text( label_schedule_date, "01.01");
        lv_obj_align( label_schedule_date, LV_ALIGN_TOP_LEFT, i*(740 / 7) + 30, 30);

    }


    static lv_style_t style_schedule;
    lv_style_init(&style_schedule);
    lv_style_set_text_font(&style_schedule, &lv_font_montserrat_14);

    static lv_style_t style_schedule_1;
    lv_style_init(&style_schedule_1);
    lv_style_set_text_font(&style_schedule_1, &lv_font_montserrat_12);

    //printf("%s", compose_time_frame(schedule[0].start,schedule[0].end));

    lv_style_set_line_width(&style_line, 1);

    static lv_style_t style_day_time;
    lv_style_init(&style_day_time);

    lv_style_set_text_font(&style_day_time, &lv_font_montserrat_12);

    static lv_point_t line_points_time[][8] = {
            {{30,   54}, {750, 54}},
            {{30,   98}, {750, 98}},
            {{30,   144}, {750, 144}},
            {{30,   189}, {750, 189}},
            {{30,   234}, {750, 234}},
            {{30,   279}, {750, 279}},
            {{30,   324}, {750, 324}},
            {{30,   369}, {750, 369}},
    };

    for(int i = 0; i < 8; i++){
        lv_obj_t * line;
        /*Create a line and apply the new style*/
        line = lv_line_create(calender_container);
        lv_line_set_points(line, line_points_time[i], 2);     /*Set the points*/
        lv_obj_add_style(line, &style_line, 0);

        char* time = (char*)malloc(sizeof(char*));
        sprintf(time, "%s", leadingZero((i*2) + 6));

        lv_obj_t *label_day_time = lv_label_create( calender_container);
        lv_obj_add_style(label_day_time,&style_day_time,0);
        lv_obj_set_width(label_day_time, 95);
        lv_obj_set_align(label_day_time, LV_TEXT_ALIGN_RIGHT);
        lv_label_set_text( label_day_time, time);
        lv_obj_align( label_day_time, LV_ALIGN_TOP_LEFT, 5, line_points_time[i][0].y-6);
    }


    for(int i = 0; i < sizeof(schedule)/sizeof(schedule[0]); i++){
        const int startPos = time_to_pos(&schedule[i].start);
        const int endPos = time_to_pos(&schedule[i].end);

        lv_obj_t * schedule_container = lv_obj_create(calender_container);
        lv_obj_set_height(schedule_container, endPos-startPos);
        lv_obj_set_width(schedule_container, 95);
        lv_obj_add_style(schedule_container, &calender_container_style, 0);
        lv_obj_align(schedule_container, LV_ALIGN_TOP_LEFT,
                     schedule[i].start.weekday * (740/7) + 30, startPos + 64);

        lv_obj_t *label_schedule_name = lv_label_create( schedule_container);
        lv_obj_add_style(label_schedule_name,&style_schedule,0);
        lv_obj_set_width(label_schedule_name, 80);
        lv_label_set_long_mode(label_schedule_name, LV_LABEL_LONG_CLIP);
        lv_label_set_text( label_schedule_name, schedule[i].name);
        lv_obj_set_align(label_schedule_name, LV_TEXT_ALIGN_CENTER);
        lv_obj_align( label_schedule_name, LV_ALIGN_DEFAULT, 5, 5);

        lv_obj_t *label_schedule_data = lv_label_create( schedule_container);
        lv_obj_add_style(label_schedule_data,&style_schedule_1,0);
        lv_label_set_text( label_schedule_data, compose_time_frame(&schedule[i].start,&schedule[i].end));
        lv_obj_set_align(label_schedule_data, LV_TEXT_ALIGN_CENTER);
        lv_obj_align( label_schedule_data, LV_ALIGN_DEFAULT, 5, 20);
    }

    return *lv_obj_get_screen(lv_scr_act());
}

lv_obj_t UIHandler::drawFirstScreen(
        weather_data* weatherData,
        int8_t hour,
        int8_t minute,
        int8_t day,
        int8_t month,
        int16_t year
) {
    static lv_style_t style_top;
    lv_style_init(&style_top);
    lv_style_set_text_font(&style_top, &lv_font_montserrat_48);

    lv_obj_t *label_time = lv_label_create( lv_scr_act());
    lv_obj_add_style(label_time,&style_top,0);
    lv_label_set_text( label_time, composeTime(hour,minute));
    lv_obj_set_align(label_time, LV_TEXT_ALIGN_RIGHT);
    lv_obj_align( label_time, LV_ALIGN_TOP_LEFT, 20, 10 );

    static lv_style_t style2;
    lv_style_init(&style2);
    lv_style_set_text_font(&style2, &lv_font_montserrat_26);

    lv_obj_t *label_date = lv_label_create( lv_scr_act());
    lv_obj_add_style(label_date,&style2,0);
    lv_label_set_text( label_date, composeDate(day,month,year));
    lv_obj_set_align(label_date, LV_TEXT_ALIGN_RIGHT);
    lv_obj_align( label_date, LV_ALIGN_TOP_LEFT, 20, 60 );

    static lv_style_t weather_container_style;
    lv_style_init(&weather_container_style);
    lv_style_set_border_color(&weather_container_style, lv_color_black());
    lv_style_set_pad_all(&weather_container_style,0);

    lv_obj_t * info_container;
    info_container = lv_obj_create(lv_scr_act());
    lv_obj_set_height(info_container, 480 - 120);
    lv_obj_set_width(info_container, 800 - 40);
    lv_obj_add_style(info_container, &weather_container_style, 0);
    lv_obj_align(info_container, LV_ALIGN_CENTER, 0, 40);

    uint8_t lineCount = 0;

    LV_IMG_DECLARE(snow);
    LV_IMG_DECLARE(clouds);
    LV_IMG_DECLARE(sunny);

    for(int i = 0; i < 4; i++){
        static lv_style_t style_weather;
        lv_style_init(&style_weather);

        static lv_style_t style_temp;
        lv_style_init(&style_temp);

        lv_style_set_text_align(&style_temp,LV_TEXT_ALIGN_CENTER);
        lv_style_set_text_font(&style_temp, &lv_font_montserrat_20);

        lv_style_set_text_align(&style_weather,LV_TEXT_ALIGN_CENTER);
        lv_style_set_text_font(&style_weather, &lv_font_montserrat_28);

        if((i+1) % 3 == 0)  lineCount++;

        lv_obj_t * img1 = lv_img_create(info_container);
        setSource(weatherData[i].weather,img1);
        lv_obj_align(img1,  LV_ALIGN_DEFAULT, (i % 2)*(800 / 4) + 55, lineCount*(360 / 2) + 65);

        char* day_str = (char*)malloc(256);
        sprintf(day_str, "%s", weatherData[0].date);

        lv_obj_t *label_weather_date = lv_label_create(info_container);
        lv_obj_add_style(label_weather_date,&style_weather,0);
        lv_label_set_text( label_weather_date, day_str);
        lv_obj_set_width(label_weather_date,100);
        lv_obj_set_height(label_weather_date,25);
        lv_obj_align( label_weather_date, LV_ALIGN_DEFAULT, (i % 2)*(800 / 4) + 30, lineCount*(360 / 2) + 20);

        free(day_str);

        static lv_style_t temperature_text_style;
        lv_style_init(&temperature_text_style);
        lv_style_set_border_color(&temperature_text_style, lv_color_black());
        lv_style_set_pad_all(&temperature_text_style,0);
        lv_style_set_radius(&temperature_text_style,100);

        lv_obj_t * temperature_text_container;
        temperature_text_container = lv_obj_create(info_container);
        lv_obj_set_height(temperature_text_container, 25);
        lv_obj_set_width(temperature_text_container, 80);
        lv_obj_add_style(temperature_text_container, &weather_container_style, 0);
        lv_obj_set_scrollbar_mode(temperature_text_container, LV_SCROLLBAR_MODE_OFF);
        lv_obj_align( temperature_text_container, LV_ALIGN_DEFAULT, (i % 2)*(800 / 4) + 40, lineCount*(360 / 2) + (110));

        char* temperature = (char*)malloc(256);
        sprintf(temperature, "%d °C", (int)weatherData[i].temp);

        lv_obj_t *label_temperature = lv_label_create( temperature_text_container);
        lv_obj_add_style(label_temperature,&style_temp,0);
        lv_label_set_text( label_temperature, temperature);
        lv_obj_set_width(label_temperature,80);
        lv_obj_set_height(label_temperature,25);

        free(temperature);
    }

    static lv_point_t line_points_time[][8] = {
            {{380,   20}, {380, 340}},
    };

    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 2);
    lv_style_set_line_color(&style_line, lv_color_make(0,0,0));
    lv_style_set_line_rounded(&style_line, true);

    lv_obj_t * line;
    /*Create a line and apply the new style*/
    line = lv_line_create(info_container);
    lv_line_set_points(line, line_points_time[0], 2);     /*Set the points*/
    lv_obj_add_style(line, &style_line, 0);

    // draw a list of news. Max of 3 news articles - the most important 3 news articles.
    static news_item_t news_items[][3] = {
            {"News Headline 1 that is super mega extra long","Keywords 1, Keyword 2 (ChatGPT)","Mo"},
            {"News Headline 2","Keywords 1, Keyword 2 (ChatGPT)","So"},
            {"News Headline 3","Keywords 1, Keyword 2 (ChatGPT)","So"}
    };

    for(int i = 0; i < 3; i++){
        static lv_style_t style_headline;
        lv_style_init(&style_headline);

        static lv_style_t style_description;
        lv_style_init(&style_description);

        lv_style_set_text_align(&style_headline,LV_TEXT_ALIGN_LEFT);
        lv_style_set_text_font(&style_headline, &lv_font_montserrat_20);

        lv_style_set_text_align(&style_description,LV_TEXT_ALIGN_LEFT);
        lv_style_set_text_font(&style_description, &lv_font_montserrat_16);

        lv_obj_t *label_headline = lv_label_create(info_container);
        lv_label_set_long_mode(label_headline, LV_LABEL_LONG_CLIP);
        lv_obj_add_style(label_headline, &style_headline, 0);
        lv_label_set_text(label_headline, news_items[i]->title);
        lv_obj_set_width(label_headline, 340);
        lv_obj_set_height(label_headline, 25);
        lv_obj_align(label_headline, LV_ALIGN_DEFAULT, 400, i * (360 / 3) + 20);

        lv_obj_t *label_description = lv_label_create(info_container);
        lv_obj_add_style(label_description, &style_description, 0);
        lv_label_set_text(label_description, news_items[i]->description);
        lv_obj_set_width(label_description, 340);
        lv_obj_set_height(label_description, 25);
        lv_obj_align(label_description, LV_ALIGN_DEFAULT, 400, i * (360 / 3) + 50);

    }

    static lv_point_t list_separator[][8] = {
            {{400,(360 / 3) * 1}, {740, (360 / 3) * 1}},
            {{400,(360 / 3) * 2}, {740, (360 / 3) * 2}}
    };

    for(int i = 0; i < 2; i++){
        lv_obj_t * lineSeparator;

        /*Create a line and apply the new style*/
        lineSeparator = lv_line_create(info_container);
        lv_line_set_points(lineSeparator, list_separator[i], 2);     /*Set the points*/
        lv_obj_add_style(lineSeparator, &style_line, 0);
    }

    return *lv_obj_get_screen(lv_scr_act());
}


lv_obj_t UIHandler::drawFirstScreenTest(
        weather_data* weatherData,
        int8_t hour,
        int8_t minute,
        int8_t day,
        int8_t month,
        int16_t year
) {
    static lv_style_t style1;
    lv_style_init(&style1);
    lv_style_set_text_font(&style1, &lv_font_montserrat_48);

    lv_obj_t * scr1 = lv_obj_create(lv_scr_act());

    lv_obj_t *label_time = lv_label_create( scr1);
    lv_obj_add_style(label_time,&style1,0);
    lv_label_set_text( label_time, (const char*)composeTime(hour,minute));
    lv_obj_set_align(label_time, LV_TEXT_ALIGN_CENTER);
    lv_obj_align( label_time, LV_ALIGN_TOP_MID, 0, 20 );

    static lv_style_t style2;
    lv_style_init(&style2);
    lv_style_set_text_font(&style2, &lv_font_montserrat_28);

    lv_obj_t *label_date = lv_label_create( scr1);
    lv_obj_add_style(label_date,&style2,0);
    lv_label_set_text( label_date, (const char*)composeDate(day,month,year) );
    lv_obj_set_align(label_date, LV_TEXT_ALIGN_CENTER);
    lv_obj_align( label_date, LV_ALIGN_TOP_MID, 0, 80 );

    static lv_style_t weather_container_style;
    lv_style_init(&weather_container_style);
    lv_style_set_border_color(&weather_container_style, lv_color_black());
    lv_style_set_height(&weather_container_style, 480 - 40);
    lv_style_set_width(&weather_container_style, 800 - 200);
    lv_style_set_pad_all(&weather_container_style,0);

    lv_obj_t * weather_container;
    weather_container = lv_obj_create(scr1);
    lv_obj_set_height(weather_container, 480 - 150);
    lv_obj_set_width(weather_container, 800 - 40);
    lv_obj_add_style(weather_container, &weather_container_style, 0);
    lv_obj_align(weather_container, LV_ALIGN_CENTER, 0, 50);

    lv_obj_t *label_location = lv_label_create( weather_container);
    lv_obj_add_style(label_location,&style2,0);
    lv_label_set_text( label_location, "Weather - St.Poelten" );
    lv_obj_set_align(label_location, LV_TEXT_ALIGN_CENTER);
    lv_obj_align( label_location, LV_ALIGN_BOTTOM_MID, 0, -30 );

    for(int i = 0; i < 4; i++){
//        Serial.println(
//                String((const char*)weatherData[i].date)
//                + " is " + String(weatherData[i].temp) + " [" + String(weatherData[i].weather) + "]"
//                + " and the max is " + String(weatherData[i].highest)
//        );

        LV_IMG_DECLARE(snow);
        LV_IMG_DECLARE(clouds);
        LV_IMG_DECLARE(sunny);

        lv_obj_t * img1 = lv_img_create(weather_container);
        setSource(weatherData[i].weather,img1);
        lv_obj_align(img1,  LV_ALIGN_DEFAULT, i*(800 / 4) + 30, 165 - 110 + 30);

        static lv_style_t style_weather;
        lv_style_init(&style_weather);

        static lv_style_t style_date;
        lv_style_init(&style_date);

        static lv_style_t style_temp;
        lv_style_init(&style_temp);

        lv_style_set_text_font(&style_weather, (i == 0)?&lv_font_montserrat_20:&lv_font_montserrat_16);

        lv_obj_t *label_weather = lv_label_create( weather_container);
        lv_obj_add_style(label_weather,&style_weather,0);
        lv_label_set_text( label_weather, weatherData[i].weather);
        lv_obj_set_align(label_weather, LV_TEXT_ALIGN_CENTER);
        lv_obj_align( label_weather, LV_ALIGN_DEFAULT, i*(800 / 4) + 30, 165 + 18 + 30);

        lv_style_set_text_font(&style_date, &lv_font_montserrat_24);

        lv_obj_t *label_weather_date = lv_label_create( weather_container);
        lv_obj_add_style(label_weather_date,&style_date,0);
        lv_label_set_text( label_weather_date, weatherData[i].date);
        lv_obj_set_align(label_weather_date, LV_TEXT_ALIGN_CENTER);
        lv_obj_align( label_weather_date, LV_ALIGN_DEFAULT, i*(800 / 4) + 30, 30);

        lv_style_set_text_font(&style_temp, (i == 0)?&lv_font_montserrat_40:&lv_font_montserrat_36);
        lv_obj_t *label_temperature = lv_label_create( weather_container);
        lv_obj_add_style(label_temperature,&style_temp,0);
        lv_label_set_text( label_temperature, String(String(weatherData[i].temp) + " °C").c_str());
        lv_obj_set_align(label_temperature, LV_TEXT_ALIGN_CENTER);
        lv_obj_align( label_temperature, LV_ALIGN_DEFAULT, i*(800 / 4) + 30, 165 - 36 + 30);
    }

    return *scr1;
}



UIHandler::UIHandler() {

}

void UIHandler::init() {
    LV_IMG_DECLARE(snow);
    LV_IMG_DECLARE(clouds);
    LV_IMG_DECLARE(sunny);

    imgSnow = lv_img_create(NULL);
    imgClouds = lv_img_create(NULL);
    imgSun = lv_img_create(NULL);

    lv_img_set_src(imgSnow, &snow);
    lv_img_set_src(imgClouds, &clouds);
    lv_img_set_src(imgSun, &day_sunny_icon);
}



