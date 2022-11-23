#include <Arduino.h>
#include <string.h>
#include "main.h"
#include "ui/ui.h"
#include "app/stoper/stoper.h"



// Settings list - AppStoper
lv_obj_t * ui_stoperScreen;
lv_obj_t * ui_stoperHeader;
lv_obj_t * ui_stoperbtn;

static void ui_stoperScreen_event(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_disp_t *display = lv_disp_get_default();
    lv_obj_t *actScr = lv_disp_get_scr_act(display);

    if (code == LV_EVENT_CLICKED){
        if (obj == ui_stoperbtn){
            openSettings();
        }
    }
}

void openAppStoper(){
    lv_obj_set_parent(ui_app_statusBar(), ui_stoperScreen);
    lv_disp_load_scr(ui_stoperScreen);
}

void AppStoper(){
    ui_stoperScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_stoperScreen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(ui_stoperScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_stoperScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_add_event_cb(ui_displayScreen, event_handler, LV_EVENT_SCREEN_UNLOADED, NULL);

    ui_stoperHeader = create_header(ui_stoperScreen, "Stoper");

    ui_stoperbtn = create_button(ui_stoperScreen, ui_stoperScreen_event);  
}