#include <Arduino.h>
#include <epaper_libs/epd7in3g.h>
#include "lvgl.h"
#include "font/lv_font.h"
#include "handlers/ConnectivityHandler/ConnectivityHandler.h"
#include "ArduinoJson.h"
#include "soc/rtc_wdt.h"
#include "http/HttpClient/AsyncHTTPClient.h"
#include "handlers/UIHandler/UIHandler.h"
#include "types/typedefs.h"


const uint8_t port = 80;

Epd epd;
ConnectivityHandler connectivityHandler = ConnectivityHandler();
UIHandler uiHandler = UIHandler();

weather_data* weatherArray;
static uint8_t currentScreen = 0;

#define COLOR_RED       0xFF
#define COLOR_BLACK     0x00
#define COLOR_WHITE     0x55
#define COLOR_YELLOW    0xAA

#define px_red          3
#define px_yellow       2
#define px_white        1
#define px_black        0

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 800;
static const uint16_t screenHeight = 480;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * 20 ];

int counter = 0;

void renderFirstScreenTask();

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    UWORD h,w;

    for(h = area->y1; h <= area->y2; h++) {
        for(w = area->x1; w < area->x2; w++) {
            if(w % 4 == 0){
                uint8_t px = 0;
                for(int i = 0; i < 4; i++){
                    uint8_t brightness = lv_color_to1(*color_p);
                    if(brightness == 0){
                        px |= px_black << (8 - (2*(i+1)));
                        // 00000000 = 0 shifted ->
                        //epd.SendData(COLOR_BLACK);
                    }
                    else {
                        px |= px_white << (8 - (2*(i+1)));
                        //epd.SendData(COLOR_WHITE);
                    }
                    color_p++;
                }
                epd.SendData(px);
            }
        }
    }

    lv_disp_t * disp1 = _lv_refr_get_disp_refreshing();
    lv_disp_flush_ready(disp1->driver);
}

void initEpaper(){
    if (epd.Init() != 0) {
        Serial.print("e-Paper init failed");
        return;
    }
}

void initLVGL(){

    lv_init();
    lv_disp_draw_buf_init( &draw_buf, buf, nullptr, screenWidth * 20);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv);

}

void renderBufOnDisplay(){

    epd.SendCommand(0x04);
    epd.ReadBusyH();

    epd.SendCommand(0x10);


    lv_task_handler();


    epd.TurnOnDisplay();
    Serial.println("New Buffer Rendered.");
}


void getWeather(){
    fetchAsync(
        HTTP_GET,
        "http://api.openweathermap.org/data/2.5/forecast?lat=48.20&lon=15.62&cnt=30&appid=f314ea552d5dbd5a3cc3e263bd69ca80",
        [](String res) -> void {
            Serial.println("Request Successful.");

            const size_t capacity = 2*JSON_ARRAY_SIZE(1) + 2*JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + 2*JSON_OBJECT_SIZE(4) + 2*JSON_OBJECT_SIZE(9) + 400;
            DynamicJsonBuffer jsonBuffer(capacity);


            const char* json = res.c_str();

            JsonObject& root = jsonBuffer.parseObject(json);

            String current = "";
            uint8_t currentWeatherIndex = 0;

            weatherArray = (weather_data*)malloc(sizeof(weather_data*)*4096);

            const size_t size = root["list"].size();

            for(uint8_t i = 0; i < size; i++){
                JsonObject& list_0 = root["list"][i];

                static uint8_t perDayData = 0;
                static float perDayAverage = 0.0f;

                String timestamp = String((const char*)list_0["dt_txt"]); // "2022-12-02 12:00:00"

                char* date = (char*)malloc(sizeof(char*));

                sprintf(date, "%s / %s", timestamp.substring(5,7).c_str(),timestamp.substring(8,10).c_str());

                if(current.equals("")){
                    current = timestamp.substring(8,10);
                }
                else{
                    String currentDay = timestamp.substring(8,10);
                    static float highestTemp = 0.0f;
                    static float lowestTemp = 0.0f;

                    if(current.equals(currentDay) && i != size - 1){
                        JsonObject& list_0_main = list_0["main"];

                        perDayAverage += ((float)list_0_main["temp"] - 273.15f);

                        if(highestTemp <= (float)list_0_main["temp_max"] - 273.15f)
                            highestTemp = (float)list_0_main["temp_max"] - 273.15f;

                        if(lowestTemp >= (float)list_0_main["temp_min"] - 273.15f)
                            lowestTemp = (float)list_0_main["temp_min"] - 273.15f;

                        perDayData++;
                    }
                    else {
                        JsonObject& list_0_weather_0 = list_0["weather"][0];

                        weatherArray[currentWeatherIndex] = {
                                date,
                                perDayAverage / perDayData,
                                lowestTemp,
                                highestTemp,
                                list_0_weather_0["main"],
                                list_0_weather_0["description"]
                        };

                        perDayAverage = 0.0f;
                        perDayData = 0;

                        current = timestamp.substring(8,10);
                        currentWeatherIndex++;
                    }
                }
            }

            renderFirstScreenTask();
        });

    // important ... if this is not here, sudden crashes might appear.
    rtc_wdt_feed();
}

void refresh_lvgl() {
    lv_obj_clean(lv_scr_act());
    lv_deinit();
    lv_mem_buf_free_all();
    initLVGL();
}

void renderFirstScreenTask(){
    xTaskCreate(
        [](void* params) -> void {
            //for(;;){
                //vTaskDelay(2000/portTICK_PERIOD_MS);

//                refresh_lvgl();
//                uiHandler.drawCalenderScreen();
//                renderBufOnDisplay();

                refresh_lvgl();
                uiHandler.drawFirstScreen(
                weatherArray,
                connectivityHandler.ntp.hours(),
                connectivityHandler.ntp.minutes(),
                connectivityHandler.ntp.day(),
                connectivityHandler.ntp.month(),
                connectivityHandler.ntp.year()
                );
                renderBufOnDisplay();

            //}
            //startPeriodicTask();
            vTaskDelete(nullptr);
        },
        "update_screen",
        8192,
        nullptr,
        0,
        nullptr);
}
void setup() {
    Serial.begin(115200);

    initEpaper();
    initLVGL();

    connectivityHandler.begin();
    getWeather();
}

void loop() {
    connectivityHandler.handle();
    delay(10);
}