#include <Arduino.h>
#include <string.h>
#include "main.h"
#include "ui/ui.h"
#include "ui/setup/storage.h"
#include <vector>
#include <string.h>
#include "hadware/json_psram_allocator.h"
#include "hadware/alloc.h"
#include "littleFS.h"
#include <ESP32Time.h>
#include "SD.h"

storage_config_t storage_config;


// Settings list - AppStorage
lv_obj_t * ui_storageScreen;
lv_obj_t * ui_storageHeader;
lv_obj_t * ui_storagebtn;

static void ui_storageScreen_event(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_disp_t *display = lv_disp_get_default();
    lv_obj_t *actScr = lv_disp_get_scr_act(display);

    if (code == LV_EVENT_CLICKED){
        if (obj == ui_storagebtn){
            openSettings();
        }
    }
}

void openAppStorage(){
    lv_obj_set_parent(ui_app_statusBar(), ui_storageScreen);
    lv_disp_load_scr(ui_storageScreen);
}

void AppStorage(){
    ui_storageScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_storageScreen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(ui_storageScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_storageScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_add_event_cb(ui_displayScreen, event_handler, LV_EVENT_SCREEN_UNLOADED, NULL);

    ui_storageHeader = create_header(ui_storageScreen, "Storage");

    ui_storagebtn = create_button(ui_storageScreen, ui_storageScreen_event);  

    
    lv_obj_t * ui_storagelabel;
    char temp[40]="";
    ui_storagelabel = create_label(ui_storageScreen, 0, "Flash", 20, 100, 20);
    uint64_t total = ESP.getFlashChipSize()/1024;
    uint64_t free = LittleFS.totalBytes()/1024;
    uint64_t ussd = LittleFS.usedBytes()/1024;
    uint64_t used = total - free - ussd;
    lv_snprintf( temp, sizeof( temp ), "%llu kB / %llu kB", used, total );
    ui_storagelabel = create_label(ui_storageScreen, 0, temp, 40, 125, 14);

    lv_obj_t * ui_storagebar;
    ui_storagebar = lv_bar_create(ui_storageScreen);
    lv_obj_set_size(ui_storagebar, 280, 20);
    lv_obj_align(ui_storagebar,LV_ALIGN_TOP_LEFT,20,150);
    int32_t val = map(used, 0, total, 0, 100);
    lv_bar_set_value(ui_storagebar, val, LV_ANIM_OFF);
    lv_obj_set_style_radius(ui_storagebar, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_storagebar, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    
    ui_storagelabel = create_label(ui_storageScreen, 0, "SD Card", 20, 200, 20);
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    uint64_t usedSize = SD.usedBytes() / (1024 * 1024);
    lv_snprintf( temp, sizeof( temp ), "%llu MB / %llu MB", usedSize, cardSize );
    ui_storagelabel = create_label(ui_storageScreen, 0, temp, 40, 225, 14);
    
    ui_storagebar = lv_bar_create(ui_storageScreen);
    lv_obj_set_size(ui_storagebar, 280, 20);
    lv_obj_align(ui_storagebar,LV_ALIGN_TOP_LEFT,20,250);
    int32_t valsd = map(usedSize, 0, cardSize, 0, 100);
    lv_bar_set_value(ui_storagebar, valsd, LV_ANIM_OFF);
    lv_obj_set_style_radius(ui_storagebar, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_storagebar, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    

}