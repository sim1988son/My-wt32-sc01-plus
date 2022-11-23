#include <Arduino.h>
#include <string.h>
#include "main.h"
#include "ui/ui.h"
#include "ui/setup/SIM.h"


// Settings list - AppSIM
lv_obj_t * ui_simScreen;
lv_obj_t * ui_simHeader;
lv_obj_t * ui_simbtn;

static void ui_simScreen_event(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_disp_t *display = lv_disp_get_default();
    lv_obj_t *actScr = lv_disp_get_scr_act(display);

    if (code == LV_EVENT_CLICKED){
        if (obj == ui_simbtn){
            openSettings();
        }
    }
}

void openAppSIM(){
    lv_obj_set_parent(ui_app_statusBar(), ui_simScreen);
    lv_disp_load_scr(ui_simScreen);
}

void AppSIM(){
    ui_simScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_simScreen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(ui_simScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_simScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_add_event_cb(ui_displayScreen, event_handler, LV_EVENT_SCREEN_UNLOADED, NULL);

    ui_simHeader = create_header(ui_simScreen, "SIM");

    ui_simbtn = create_button(ui_simScreen, ui_simScreen_event);  
}
