#include <Arduino.h>
#include <string.h>
#include "main.h"
#include "ui/ui.h"
#include "ui/setup/sound.h"



// Settings list - AppSound
lv_obj_t * ui_soundScreen;
lv_obj_t * ui_soundHeader;
lv_obj_t * ui_soundbtn;

static void ui_soundScreen_event(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_disp_t *display = lv_disp_get_default();
    lv_obj_t *actScr = lv_disp_get_scr_act(display);

    if (code == LV_EVENT_CLICKED){
        if (obj == ui_soundbtn){
            openSettings();
        }
    }
}

void openAppSound(){
    lv_obj_set_parent(ui_app_statusBar(), ui_soundScreen);
    lv_disp_load_scr(ui_soundScreen);
}

void AppSound(){
    ui_soundScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_soundScreen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(ui_soundScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_soundScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_add_event_cb(ui_displayScreen, event_handler, LV_EVENT_SCREEN_UNLOADED, NULL);

    ui_soundHeader = create_header(ui_soundScreen, "Sound");

    ui_soundbtn = create_button(ui_soundScreen, ui_soundScreen_event);  
}