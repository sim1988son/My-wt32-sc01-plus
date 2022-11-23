#include <Arduino.h>
#include <string.h>
#include "main.h"
#include "ui/ui.h"
#include <WiFi.h>
#include "app/weather/weather.h"



// Settings list - AppWeather
lv_obj_t * ui_weatherScreen;
lv_obj_t * ui_weatherHeader;
lv_obj_t * ui_weatherbtn;

static void ui_weatherScreen_event(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_disp_t *display = lv_disp_get_default();
    lv_obj_t *actScr = lv_disp_get_scr_act(display);

    if (code == LV_EVENT_CLICKED){
        if (obj == ui_weatherbtn){
            openSettings();
        }
    }
}

void openAppWeatherSetup(){
    lv_obj_set_parent(ui_app_statusBar(), ui_weatherScreen);
    lv_disp_load_scr(ui_weatherScreen);
}

void AppWeatherSetup(){
    ui_weatherScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_weatherScreen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(ui_weatherScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_weatherScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_add_event_cb(ui_displayScreen, event_handler, LV_EVENT_SCREEN_UNLOADED, NULL);

    ui_weatherHeader = create_header(ui_weatherScreen, "Weather");

    ui_weatherbtn = create_button(ui_weatherScreen, ui_weatherScreen_event);  
}
