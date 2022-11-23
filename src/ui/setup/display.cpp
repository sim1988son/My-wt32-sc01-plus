#include <Arduino.h>
#include "main.h"
#include "ui/ui.h"
#include "ui/setup/display.h"
#include <vector>
#include <string.h>
#include "hadware/json_psram_allocator.h"
#include "hadware/alloc.h"
#include "littleFS.h"
#include <ESP32Time.h>

display_config_t display_config;

// Settings list - AppDisplay
lv_obj_t * ui_displayScreen;
lv_obj_t * ui_displayHeader;
lv_obj_t * ui_displaybtn;
lv_obj_t * ui_displaylabel;
lv_obj_t * ui_displayswitch;
lv_obj_t * ui_displayslider;

static void ui_displayScreen_event(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_disp_t *display = lv_disp_get_default();
    lv_obj_t *actScr = lv_disp_get_scr_act(display);

    if (code == LV_EVENT_CLICKED){
        if (obj == ui_displaybtn){
            display_save_config();
            openSettings();
        }
    }
    if (code == LV_EVENT_VALUE_CHANGED){

    }
}

static void ui_displaySlider_event(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    
    if (code == LV_EVENT_VALUE_CHANGED){
        display_set_display_brig((int)lv_slider_get_value(obj));
        tft_setbrightness((int)lv_slider_get_value(obj));
    }
}

void openAppDisplay(){
    lv_obj_set_parent(ui_app_statusBar(), ui_displayScreen);
    lv_disp_load_scr(ui_displayScreen);
}

void AppDisplay(){

    display_load_config();

    ui_displayScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_displayScreen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(ui_displayScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_displayScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_add_event_cb(ui_displayScreen, event_handler, LV_EVENT_SCREEN_UNLOADED, NULL);

    ui_displayHeader = create_header(ui_displayScreen, "Display");

    ui_displaybtn = create_button(ui_displayScreen, ui_displayScreen_event);  

    ui_displaylabel = create_label(ui_displayScreen,0, "Jasnosc ekranu", 20, 100, 20);
    ui_displayslider = create_slider(ui_displayScreen, ui_displaySlider_event, 20, 150, 280, 5);
    lv_slider_set_range(ui_displayslider, 8 , 255);
    lv_slider_set_value(ui_displayslider, display_get_display_brig(), LV_ANIM_OFF);

    ui_displaylabel = create_label(ui_displayScreen,0, "Automatycznie" , 20, 200, 20);
    ui_displayswitch = create_switch(ui_displayScreen, ui_displayScreen_event, 200);
    lv_obj_add_state(ui_displayswitch, LV_STATE_DISABLED);
    
    tft_setbrightness((int)lv_slider_get_value(ui_displayslider));
}


void display_save_config( void ) {
    fs::File file = LittleFS.open( DISPLAYSETUP_JSON_CONFIG_FILE, FILE_WRITE );
    if (!file) {
        log_e("Can't open file: %s!", DISPLAYSETUP_JSON_CONFIG_FILE );
    }else {
        SpiRamJsonDocument doc( 1000 );
        doc["display_auto"] = display_config.display_auto;
        doc["display_brig"] = display_config.display_brig;
        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

void display_load_config( void ) {
    fs::File file = LittleFS.open( DISPLAYSETUP_JSON_CONFIG_FILE, FILE_READ );
    if (!file) {
        log_e("Can't open file: %s!", DISPLAYSETUP_JSON_CONFIG_FILE );
    }else {
        int filesize = file.size();
        SpiRamJsonDocument doc( filesize * 2 );
        DeserializationError error = deserializeJson( doc, file );
        if ( error ) {
            log_e("update check deserializeJson() failed: %s", error.c_str() );
        }
        else {
            display_config.display_auto = doc["display_auto"] | true;
            display_config.display_brig = doc["display_brig"] | 1;
        }        
        doc.clear();
    }
    file.close();
}

bool display_get_display_auto( void ) {
    return( display_config.display_auto );
}

void display_set_display_auto( bool display_auto ) {
    display_config.display_auto = display_auto;
}

int32_t display_get_display_brig( void ) {
    return( display_config.display_brig );
}

void display_set_display_brig( int32_t display_brig ) {
    display_config.display_brig = display_brig;
}
