#include <Arduino.h>
#include <string.h>
#include "main.h"
#include "ui/ui.h"
#include "app/alarm/alarm.h"



// Settings list - AppAlarm
lv_obj_t * ui_alarmScreen;
lv_obj_t * ui_alarmHeader;
lv_obj_t * ui_alarmbtn;

static void ui_alarmScreen_event(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_disp_t *display = lv_disp_get_default();
    lv_obj_t *actScr = lv_disp_get_scr_act(display);

    if (code == LV_EVENT_CLICKED){
        if (obj == ui_alarmbtn){
            openSettings();
        }
    }
}

void openAppAlarmSetup(){
    lv_obj_set_parent(ui_app_statusBar(), ui_alarmScreen);
    lv_disp_load_scr(ui_alarmScreen);
}

void AppAlarmSetup(){
    ui_alarmScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_alarmScreen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(ui_alarmScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_alarmScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_add_event_cb(ui_displayScreen, event_handler, LV_EVENT_SCREEN_UNLOADED, NULL);

    ui_alarmHeader = create_header(ui_alarmScreen, "Alarm");
    ui_alarmbtn = create_button(ui_alarmScreen, ui_alarmScreen_event);  

}
