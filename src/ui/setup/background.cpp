#include <Arduino.h>
#include <string.h>
#include "main.h"
#include "ui/ui.h"
#include <WiFi.h>
#include "ui/setup/background.h"



// Settings list - AppBackground
lv_obj_t * ui_backgroundScreen;
lv_obj_t * ui_backgroundHeader;
lv_obj_t * ui_backgroundbtn;

static void ui_backgroundScreen_event(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_disp_t *display = lv_disp_get_default();
    lv_obj_t *actScr = lv_disp_get_scr_act(display);

    if (code == LV_EVENT_CLICKED){
        if (obj == ui_backgroundbtn){
            openSettings();
        }
    }
}

void openAppBackground(){
    lv_obj_set_parent(ui_app_statusBar(), ui_backgroundScreen);
    lv_disp_load_scr(ui_backgroundScreen);
}

void AppBackground(){
    ui_backgroundScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_backgroundScreen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(ui_backgroundScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_backgroundScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_add_event_cb(ui_displayScreen, event_handler, LV_EVENT_SCREEN_UNLOADED, NULL);

    ui_backgroundHeader = create_header(ui_backgroundScreen, "Background");

    ui_backgroundbtn = create_button(ui_backgroundScreen, ui_backgroundScreen_event);  
}

