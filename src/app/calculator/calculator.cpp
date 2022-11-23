#include <Arduino.h>
#include <string.h>
#include "main.h"
#include "ui/ui.h"
#include "app/calculator/calculator.h"





// Settings list - AppCalculator
lv_obj_t * ui_calculatorScreen;
lv_obj_t * ui_calculatorHeader;
lv_obj_t * ui_calculatorbtn;

static void ui_calculatorScreen_event(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_disp_t *display = lv_disp_get_default();
    lv_obj_t *actScr = lv_disp_get_scr_act(display);

    if (code == LV_EVENT_CLICKED){
        if (obj == ui_calculatorbtn){
            openSettings();
        }
    }
}

void openAppCalculatorSetup(){
    lv_obj_set_parent(ui_app_statusBar(), ui_calculatorScreen);
    lv_disp_load_scr(ui_calculatorScreen);
}

void AppCalculatorSetup(){
    ui_calculatorScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_calculatorScreen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(ui_calculatorScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_calculatorScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_add_event_cb(ui_displayScreen, event_handler, LV_EVENT_SCREEN_UNLOADED, NULL);

    ui_calculatorHeader = create_header(ui_calculatorScreen, "Calculator");
    ui_calculatorbtn = create_button(ui_calculatorScreen, ui_calculatorScreen_event);  

}