#include <Arduino.h>
#include <string.h>
#include "main.h"
#include "ui/ui.h"
#include "ui/setup/battery.h"
#include <vector>
#include <string.h>
#include "hadware/json_psram_allocator.h"
#include "hadware/alloc.h"
#include "littleFS.h"
#include <ESP32Time.h>

battery_config_t battery_config;

// Settings list - AppBattery
lv_obj_t * ui_batteryScreen;
lv_obj_t * ui_batteryHeader;
lv_obj_t * ui_batterybtn;
lv_obj_t * ui_batterylabel;

static void ui_batteryScreen_event(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_disp_t *display = lv_disp_get_default();
    lv_obj_t *actScr = lv_disp_get_scr_act(display);

    if (code == LV_EVENT_CLICKED){
        if (obj == ui_batterybtn){
            openSettings();
        }
    }
}

void openAppBattery(){
    lv_obj_set_parent(ui_app_statusBar(), ui_batteryScreen);
    lv_disp_load_scr(ui_batteryScreen);
}

void AppBattery(){
    ui_batteryScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_batteryScreen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(ui_batteryScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_batteryScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_add_event_cb(ui_displayScreen, event_handler, LV_EVENT_SCREEN_UNLOADED, NULL);

    ui_batteryHeader = create_header(ui_batteryScreen, "Battery");

    ui_batterybtn = create_button(ui_batteryScreen, ui_batteryScreen_event);  

    ui_batterylabel = create_label(ui_batteryScreen,0, "28%", 20, 100, 20);

    lv_obj_t * ui_displaswitch = create_switch(ui_batteryScreen, ui_batteryScreen_event, 150);
    
    lv_obj_t * ui_displalabel = create_label(ui_batteryScreen,0, "oszczedzanie batt", 20, 150, 20);

    lv_obj_t * ui_displaslider = create_slider(ui_batteryScreen, ui_batteryScreen_event, 20, 200, 280, 5);
    
    /*Create a chart*/
    lv_obj_t * chart;
    chart = lv_chart_create(ui_batteryScreen);
    lv_obj_set_size(chart, 280, 200);
    lv_obj_set_x(chart, 20);
    lv_obj_set_y(chart, 250);
    lv_obj_set_align(chart, LV_ALIGN_TOP_LEFT);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/

    /*Add two data series*/
    lv_chart_series_t * ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_SECONDARY_Y);

    /*Directly set points on 'ser2'*/
    ser2->y_points[0] = 90;
    ser2->y_points[1] = 70;
    ser2->y_points[2] = 65;
    ser2->y_points[3] = 65;
    ser2->y_points[4] = 65;
    ser2->y_points[5] = 65;
    ser2->y_points[6] = 65;
    ser2->y_points[7] = 65;
    ser2->y_points[8] = 65;
    ser2->y_points[9] = 65;

    lv_chart_refresh(chart); /*Required after direct set*/
}


void battery_save_config( void ) {
    fs::File file = LittleFS.open( BATTERYSETUP_JSON_CONFIG_FILE, FILE_WRITE );
    if (!file) {
        log_e("Can't open file: %s!", BATTERYSETUP_JSON_CONFIG_FILE );
    }else {
        SpiRamJsonDocument doc( 1000 );
        doc["battery_auto"] = battery_config.battery_auto;
        doc["battery_brig"] = battery_config.battery_brig;
        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

void battery_load_config( void ) {
    fs::File file = LittleFS.open( BATTERYSETUP_JSON_CONFIG_FILE, FILE_READ );
    if (!file) {
        log_e("Can't open file: %s!", BATTERYSETUP_JSON_CONFIG_FILE );
    }else {
        int filesize = file.size();
        SpiRamJsonDocument doc( filesize * 2 );
        DeserializationError error = deserializeJson( doc, file );
        if ( error ) {
            log_e("update check deserializeJson() failed: %s", error.c_str() );
        }
        else {
            battery_config.battery_auto = doc["battery_auto"] | true;
            battery_config.battery_brig = doc["battery_brig"] | 1;
        }        
        doc.clear();
    }
    file.close();
}

bool display_get_battery_auto( void ) {
    return( battery_config.battery_auto );
}

void display_set_battery_auto( bool battery_auto ) {
    battery_config.battery_auto = battery_auto;
}

int32_t display_get_battery_brig( void ) {
    return( battery_config.battery_brig );
}

void display_set_battery_brig( int32_t battery_brig ) {
    battery_config.battery_brig = battery_brig;
}

