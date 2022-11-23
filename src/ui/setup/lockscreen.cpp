#include <Arduino.h>
#include <string.h>
#include "main.h"
#include "ui/ui.h"
#include "ui/setup/lockscreen.h"



// Settings list - AppLockScreen
lv_obj_t * ui_lockScreenScreen;
lv_obj_t * ui_lockScreenHeader;
lv_obj_t * ui_lockScreenbtn;

static void ui_lockScreenScreen_event(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_disp_t *display = lv_disp_get_default();
    lv_obj_t *actScr = lv_disp_get_scr_act(display);

    if (code == LV_EVENT_CLICKED){
        if (obj == ui_lockScreenbtn){
            openSettings();
        }
    }
}

void openAppLockScreen(){
    lv_obj_set_parent(ui_app_statusBar(), ui_lockScreenScreen);
    lv_disp_load_scr(ui_lockScreenScreen);
}

void AppLockScreen(){
    ui_lockScreenScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_lockScreenScreen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(ui_lockScreenScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_lockScreenScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_add_event_cb(ui_displayScreen, event_handler, LV_EVENT_SCREEN_UNLOADED, NULL);

    ui_lockScreenHeader = create_header(ui_lockScreenScreen, "Lock screen");

    ui_lockScreenbtn = create_button(ui_lockScreenScreen, ui_lockScreenScreen_event);  
}
