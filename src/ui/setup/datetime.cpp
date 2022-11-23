#include <Arduino.h>
#include "main.h"
#include "ui/ui.h"
#include "ui/setup/datetime.h"
#include <WiFi.h>
#include <vector>
#include <string.h>
#include "hadware/json_psram_allocator.h"
#include "hadware/alloc.h"
#include "littleFS.h"
#include <ESP32Time.h>

datetime_config_t datetime_config;

ESP32Time rtc(0);

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 1 * 60 * 60;  // Set your timezone here
const int daylightOffset_sec = 0;

String hourMinWithSymbol;


// Settings list -AppDateTime
lv_obj_t * ui_dateTimeScreen;
lv_obj_t * ui_dateTimeHeader;
lv_obj_t * ui_dateTimebtn;
lv_obj_t * ui_dateTimeswAuto;
lv_obj_t * ui_dateTimesw24;
lv_obj_t * ui_dateTimesw24label;
//
lv_obj_t * ui_dateTimemboxTime;
lv_obj_t * ui_dateTimemboxTimebtnOK;
lv_obj_t * ui_dateTimemboxTimebtnCL;
lv_obj_t * ui_dateTimebtnTime;
lv_obj_t * ui_dateTimelabelTime;
lv_obj_t * ui_dateTimemboxDaterollhor;
lv_obj_t * ui_dateTimemboxDaterollmin;
lv_obj_t * ui_dateTimemboxDateroll24;
//
lv_obj_t * ui_dateTimemboxDate;
lv_obj_t * ui_dateTimemboxDatebtnOK;
lv_obj_t * ui_dateTimemboxDatebtnCL;
lv_obj_t * ui_dateTimelabelDate;
lv_obj_t * ui_dateTimebtnDate;
lv_obj_t * ui_dateTimemboxDaterollday;
lv_obj_t * ui_dateTimemboxDaterollmon;
lv_obj_t * ui_dateTimemboxDaterollyer;

lv_obj_t * ui_dateTimeddlabel;
lv_obj_t * ui_dateTimedd;

lv_obj_t * ui_Llabel;


static void ui_dateTimeScreen_event(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED){
        if (obj == ui_dateTimebtn){
            datetime_save_config();
            openSettings();
        }
        else if (obj == ui_dateTimebtnTime){
            lv_obj_clear_flag(ui_dateTimemboxTime, LV_OBJ_FLAG_HIDDEN);
            // bool sw_24_true = datetime_get_timesync_24hr;// lv_obj_has_state(ui_dateTimedd, LV_STATE_CHECKED);
            // // if (sw_24_true){
            //     // ui_dateTimemboxDaterollhor = create_roller(ui_dateTimemboxTime, 24, -40, -10);
            // // }else{
            // //     lv_obj_clean(ui_dateTimemboxDaterollhor);
            // //     ui_dateTimemboxDaterollhor = create_roller(ui_dateTimemboxTime, 12, -80, -10);
            // // }
            // lv_dropdown_set_selected(ui_dateTimemboxDaterollhor, rtc.getHour(sw_24_true));
            // lv_dropdown_set_selected(ui_dateTimemboxDaterollmin, rtc.getMinute());
        }
        else if (obj == ui_dateTimemboxTimebtnOK){
            lv_obj_add_flag(ui_dateTimemboxTime, LV_OBJ_FLAG_HIDDEN);
        }
        else if (obj == ui_dateTimemboxTimebtnCL){
            lv_obj_add_flag(ui_dateTimemboxTime, LV_OBJ_FLAG_HIDDEN);
        }
        else if (obj == ui_dateTimebtnDate){
            lv_obj_clear_flag(ui_dateTimemboxDate, LV_OBJ_FLAG_HIDDEN);
            // int16_t tt = rtc.getDay();
            // lv_dropdown_set_selected(ui_dateTimemboxDaterollday, tt);
            // tt = rtc.getMonth();
            // lv_dropdown_set_selected(ui_dateTimemboxDaterollmon, tt);
            // tt = rtc.getYear()-2000;
            // lv_dropdown_set_selected(ui_dateTimemboxDaterollyer, tt);
        }
        else if (obj == ui_dateTimemboxDatebtnOK){
            lv_obj_add_flag(ui_dateTimemboxDate, LV_OBJ_FLAG_HIDDEN);
        }
        else if (obj == ui_dateTimemboxDatebtnCL){
            lv_obj_add_flag(ui_dateTimemboxDate, LV_OBJ_FLAG_HIDDEN);
        }
        
    }
    if (code == LV_EVENT_VALUE_CHANGED){
        datetime_set_timesync_auto( lv_obj_has_state(ui_dateTimeswAuto, LV_STATE_CHECKED) );
        datetime_set_timesync_24hr( lv_obj_has_state(ui_dateTimesw24, LV_STATE_CHECKED) ); 
        datetime_set_timesync_GMT( lv_dropdown_get_selected(ui_dateTimedd));

        if (lv_obj_has_state(ui_dateTimeswAuto, LV_STATE_CHECKED)){
            lv_obj_add_flag(ui_dateTimebtnTime, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_dateTimesw24, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_dateTimesw24label, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_dateTimebtnDate, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_dateTimeddlabel, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_dateTimedd, LV_OBJ_FLAG_HIDDEN);
        }else{
            lv_obj_clear_flag(ui_dateTimebtnTime, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_dateTimesw24, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_dateTimesw24label, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_dateTimebtnDate, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_dateTimeddlabel, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_dateTimedd, LV_OBJ_FLAG_HIDDEN);
        }
        // if (lv_obj_has_state(ui_dateTimesw24, LV_STATE_CHECKED)){
        // //     lv_obj_clean(ui_dateTimemboxDaterollhor);
        // //     ui_dateTimemboxDaterollhor = create_roller(ui_dateTimemboxTime, 24, -80, -10);
        // // }else{
        // //     lv_obj_clean(ui_dateTimemboxDaterollhor);
        // //     ui_dateTimemboxDaterollhor = create_roller(ui_dateTimemboxTime, 12, -80, -10);
        // }
    }
}

void openAppDateTime(){
    lv_obj_set_parent(ui_app_statusBar(), ui_dateTimeScreen);
    lv_disp_load_scr(ui_dateTimeScreen);
}

void AppDateTime(){

    datetime_load_config();

    lv_obj_t * ui_dateTimelabel;

    ui_dateTimeScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_dateTimeScreen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(ui_dateTimeScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_dateTimeScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_add_event_cb(ui_dateTimeScreen, event_handler, LV_EVENT_SCREEN_UNLOADED, NULL);

    ui_dateTimeHeader = create_header(ui_dateTimeScreen, "Date & time");
    ui_dateTimebtn = create_button(ui_dateTimeScreen, ui_dateTimeScreen_event);

    ui_dateTimelabel = create_label(ui_dateTimeScreen,0, "Automatycznie", 20, 100, 20);
    ui_dateTimeswAuto = create_switch(ui_dateTimeScreen, ui_dateTimeScreen_event ,100);

    //
    ui_dateTimebtnTime = create_button_obj(ui_dateTimeScreen, ui_dateTimeScreen_event, 150);
    ui_dateTimelabel = create_label(ui_dateTimebtnTime,0, "Czas", 0, 0, 20);
    ui_dateTimelabelTime = create_label(ui_dateTimebtnTime,1, "18:40", 0, 0, 14);

    //
    ui_dateTimebtnDate = create_button_obj(ui_dateTimeScreen, ui_dateTimeScreen_event, 220);
    ui_dateTimelabel = create_label(ui_dateTimebtnDate,0, "Data",0 , 0, 20);
    ui_dateTimelabelDate = create_label(ui_dateTimebtnDate,1, "1 maj 2022",0 , 0, 14);

    ui_dateTimeddlabel = create_label(ui_dateTimeScreen,0, "Strefa czasowa", 20, 290, 20);
    ui_dateTimedd = lv_dropdown_create(ui_dateTimeScreen);
    lv_dropdown_set_options(ui_dateTimedd, "GMT+12\n"
                            "GMT+11\n"
                            "GMT+10\n"
                            "GMT+9\n"
                            "GMT+8\n"
                            "GMT+7\n"
                            "GMT+6\n"
                            "GMT+5\n"
                            "GMT+4\n"
                            "GMT+3\n"
                            "GMT+2\n"
                            "GMT+1\n"
                            "GMT+0\n"
                            "GMT-1\n"
                            "GMT-2\n"
                            "GMT-3\n"
                            "GMT-4\n"
                            "GMT-5\n"
                            "GMT-6\n"
                            "GMT-7\n"
                            "GMT-8\n"
                            "GMT-9\n"
                            "GMT-10\n"
                            "GMT-11\n"
                            "GMT-12");
    lv_obj_set_width(ui_dateTimedd, 100);  /// 1
    lv_obj_align(ui_dateTimedd, LV_ALIGN_TOP_RIGHT, -20, 285);
    // int32_t aa = datetime_get_timesync_GMT();
    lv_dropdown_set_selected(ui_dateTimedd, datetime_get_timesync_GMT());
    lv_obj_add_event_cb(ui_dateTimedd, ui_dateTimeScreen_event, LV_EVENT_VALUE_CHANGED, NULL);

    ui_dateTimesw24label = create_label(ui_dateTimeScreen, 0, "Format godziny", 20, 350, 20);
    ui_dateTimesw24 = create_switch(ui_dateTimeScreen, ui_dateTimeScreen_event, 350);
    
    ui_dateTimemboxTime = create_mbox(ui_dateTimeScreen, "Czas:", 20, 130, 280, 240);
    lv_obj_add_flag(ui_dateTimemboxTime, LV_OBJ_FLAG_HIDDEN);
    ui_dateTimemboxTimebtnOK = create_mboxbtn(ui_dateTimemboxTime, ui_dateTimeScreen_event, true);
    ui_dateTimemboxTimebtnCL = create_mboxbtn(ui_dateTimemboxTime, ui_dateTimeScreen_event, false);

    ui_dateTimemboxDaterollhor = create_roller(ui_dateTimemboxTime, 24, -40, -10);
    ui_dateTimemboxDaterollmin = create_roller(ui_dateTimemboxTime, 60, 40, -10);
    
    ui_dateTimemboxDate = create_mbox(ui_dateTimeScreen, "Data:", 20, 130, 280, 240);
    lv_obj_add_flag(ui_dateTimemboxDate, LV_OBJ_FLAG_HIDDEN);
    ui_dateTimemboxDatebtnOK = create_mboxbtn(ui_dateTimemboxDate, ui_dateTimeScreen_event, true);
    ui_dateTimemboxDatebtnCL = create_mboxbtn(ui_dateTimemboxDate, ui_dateTimeScreen_event, false);

    ui_dateTimemboxDaterollday = create_roller(ui_dateTimemboxDate, 30, -80, -10);
    ui_dateTimemboxDaterollmon = create_roller(ui_dateTimemboxDate, 12, 0, -10);
    ui_dateTimemboxDaterollyer = create_roller(ui_dateTimemboxDate, 44, 80, -10);

    lv_obj_add_flag(ui_dateTimebtnTime, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_dateTimesw24, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_dateTimesw24label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_dateTimebtnDate, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_dateTimeddlabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_dateTimedd, LV_OBJ_FLAG_HIDDEN);

    if(datetime_get_timesync_auto()){
        lv_obj_add_state(ui_dateTimeswAuto, LV_STATE_CHECKED);
        lv_event_send(ui_dateTimeswAuto, LV_EVENT_VALUE_CHANGED, NULL);
    }else{
        lv_obj_clear_state(ui_dateTimeswAuto, LV_STATE_CHECKED);
        lv_event_send(ui_dateTimeswAuto, LV_EVENT_VALUE_CHANGED, NULL);
    }

    if(datetime_get_timesync_24hr()){
        lv_obj_add_state(ui_dateTimesw24, LV_STATE_CHECKED);
        lv_event_send(ui_dateTimesw24, LV_EVENT_VALUE_CHANGED, NULL);
    }else{
        lv_obj_clear_state(ui_dateTimesw24, LV_STATE_CHECKED);
        lv_event_send(ui_dateTimesw24, LV_EVENT_VALUE_CHANGED, NULL);
    }
    
}

void updateLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }

  char hourMin[7];
  strftime(hourMin, 6, "%H:%M", &timeinfo);
  hourMinWithSymbol = String(hourMin);
}

void configlockaltime(){
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)){
        rtc.setTimeStruct(timeinfo); 
    }
}

String updatelockTime(){
    return hourMinWithSymbol;
}

void datetime_save_config( void ) {
    fs::File file = LittleFS.open( DATETIMESETUP_JSON_CONFIG_FILE, FILE_WRITE );
    if (!file) {
        log_e("Can't open file: %s!", DATETIMESETUP_JSON_CONFIG_FILE );
    }else {
        SpiRamJsonDocument doc( 1000 );
        doc["timesync_auto"] = datetime_config.timesync_auto;
        doc["timesync_24hr"] = datetime_config.timesync_24hr;
        doc["timesync_GMT"] =  datetime_config.timesync_GMT;
        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

void datetime_load_config( void ) {
    fs::File file = LittleFS.open( DATETIMESETUP_JSON_CONFIG_FILE, FILE_READ );
    if (!file) {
        log_e("Can't open file: %s!", DATETIMESETUP_JSON_CONFIG_FILE );
    }else {
        int filesize = file.size();
        SpiRamJsonDocument doc( filesize * 2 );
        DeserializationError error = deserializeJson( doc, file );
        if ( error ) {
            log_e("update check deserializeJson() failed: %s", error.c_str() );
        }
        else {
            datetime_config.timesync_auto = doc["timesync_auto"] | true;
            datetime_config.timesync_24hr = doc["timesync_24hr"] | true;
            datetime_config.timesync_GMT  = doc["timesync_GMT"] | 1;
        }        
        doc.clear();
    }
    file.close();
}

bool datetime_get_timesync_auto( void ) {
    return( datetime_config.timesync_auto );
}

void datetime_set_timesync_auto( bool timesync_auto ) {
    datetime_config.timesync_auto = timesync_auto;
}

bool datetime_get_timesync_24hr( void ) {
    return( datetime_config.timesync_24hr );
}

void datetime_set_timesync_24hr( bool timesync_24hr ) {
    datetime_config.timesync_24hr = timesync_24hr;
}

int32_t datetime_get_timesync_GMT( void ) {
    return( datetime_config.timesync_GMT );
}

void datetime_set_timesync_GMT( int32_t timesync_GMT ) {
    datetime_config.timesync_GMT = timesync_GMT;
}
