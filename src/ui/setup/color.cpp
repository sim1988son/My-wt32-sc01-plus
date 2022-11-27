#include <Arduino.h>
#include <string.h>
#include "main.h"
#include "ui/ui.h"
#include "ui/setup/color.h"
#include "hadware/json_psram_allocator.h"
#include "hadware/alloc.h"
#include "littleFS.h"

colorsetup_config_t colorsetup_config;

// Settings list - AppColors
lv_obj_t * ui_colorsScreen;
lv_obj_t * ui_colorsHeader;
lv_obj_t * ui_colorsbtn;
lv_obj_t * ui_colorslabel;
lv_obj_t * ui_colorsswAuto;
lv_obj_t * ui_colorobjlighten;
lv_obj_t * ui_colorobjlighten1;
lv_obj_t * ui_colorobjlighten2;
lv_obj_t * ui_colorobjdarken;
lv_obj_t * ui_colorobjdarken1;
lv_obj_t * ui_colorobjdarken2;
lv_obj_t * ui_colorobjcb1;
lv_obj_t * ui_colorobjcb2;
lv_obj_t * ui_colorobj_blue;
lv_obj_t * ui_colorobj_red;
lv_obj_t * ui_colorobj_yelow;
lv_obj_t * ui_colorobj_green;

bool t;

static void ui_colorsScreen_event(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED){
        if (obj == ui_colorsbtn){
            colorsetup_save_config();
            openSettings();
        }
    }
}

static void ui_colorobjcb_event(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    
    if (code == LV_EVENT_CLICKED){
        if (obj == ui_colorobjlighten){
            colorsetup_config.darkon = false;
        }
        if (obj == ui_colorobjdarken){
            colorsetup_config.darkon = true;
        }
        if (obj == ui_colorobj_blue){
            colorsetup_config.cprimary = 0x005fff;//lv_color_hex(LV_PALETTE_RED);
        }
        if (obj == ui_colorobj_red){
            colorsetup_config.cprimary = 0xcd0000;//lv_color_hex(LV_PALETTE_RED);
        }
        if (obj == ui_colorobj_yelow){
            colorsetup_config.cprimary = 0xcdcd00;//lv_color_hex(LV_PALETTE_RED);
        }
        if (obj == ui_colorobj_green){
            colorsetup_config.cprimary = 0x00cd00;//lv_color_hex(LV_PALETTE_RED);
        }
        lv_obj_set_style_bg_color(ui_colorobjlighten1, lv_color_hex(colorsetup_config.cprimary), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(ui_colorobjdarken1, lv_color_hex(colorsetup_config.cprimary), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_set_theme_color();
    }
}

void openAppColors(){
    lv_obj_set_parent(ui_app_statusBar(), ui_colorsScreen);
    lv_disp_load_scr(ui_colorsScreen);
}

void AppColors(){
    

    ui_colorsScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_colorsScreen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(ui_colorsScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_colorsScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_add_event_cb(ui_displayScreen, event_handler, LV_EVENT_SCREEN_UNLOADED, NULL);

    ui_colorsHeader = create_header(ui_colorsScreen, "Colors");

    ui_colorsbtn = create_button(ui_colorsScreen, ui_colorsScreen_event);  

    // ui_colorslabel = create_label(ui_colorsScreen,0, "Temat", 20, 350, 20);
    // ui_colorsswAuto = create_switch(ui_colorsScreen, ui_colorsScreen_event ,350);

    lv_color_t c;
    lv_color_t d;
    c = lv_theme_get_color_primary(NULL);
    d = lv_theme_get_color_secondary(NULL);

    ui_colorobjlighten = create_obj(ui_colorsScreen, -70, 80, 90, 120, lv_color_white());
    lv_obj_add_event_cb(ui_colorobjlighten, ui_colorobjcb_event, LV_EVENT_ALL , NULL);
    ui_colorobjlighten1 = create_obj(ui_colorobjlighten, -10, 65, 60, 20, c);
    ui_colorobjlighten2 = create_obj(ui_colorobjlighten, 10, 90, 60, 20, d);

    ui_colorobjdarken = create_obj(ui_colorsScreen, 70, 80, 90, 120, lv_palette_darken(LV_PALETTE_GREY,3));
    lv_obj_add_event_cb(ui_colorobjdarken, ui_colorobjcb_event, LV_EVENT_ALL , NULL);
    ui_colorobjdarken1 = create_obj(ui_colorobjdarken, -10, 65, 60, 20, c);
    ui_colorobjdarken2 = create_obj(ui_colorobjdarken, 10, 90, 60, 20, d);

    ui_colorobj_blue = create_obj_color(ui_colorsScreen, 20, 250, 50, 0x005fff, ui_colorobjcb_event);
    ui_colorobj_red = create_obj_color(ui_colorsScreen, 100, 250, 50, 0xcd0000, ui_colorobjcb_event);
    ui_colorobj_yelow = create_obj_color(ui_colorsScreen, 180, 250, 50, 0xcdcd00, ui_colorobjcb_event);
    ui_colorobj_green = create_obj_color(ui_colorsScreen, 260, 250, 50, 0x00cd00, ui_colorobjcb_event);
   

}

void lv_set_theme_color(void){ 
           
        lv_disp_t *display = lv_disp_get_default();
        lv_theme_t *theme = lv_theme_default_init(display, 
                                                lv_color_hex(colorsetup_config.cprimary), lv_palette_main(LV_PALETTE_GREY),
                                                colorsetup_config.darkon, 
                                                LV_FONT_DEFAULT);
        lv_disp_set_theme(display, theme);
}

void colorsetup_save_config( void ) {
    fs::File file = LittleFS.open( COLORSETUP_JSON_CONFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", COLORSETUP_JSON_CONFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 10000 );

        doc["darkon"] = colorsetup_config.darkon;
        doc["cprimary"] = colorsetup_config.cprimary;
        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

void colorsetup_load_config( void ) {
    fs::File file = LittleFS.open( COLORSETUP_JSON_CONFIG_FILE, FILE_READ );
    if (!file) {
        log_e("Can't open file: %s!", COLORSETUP_JSON_CONFIG_FILE );
    }
    else {
        int filesize = file.size();
        SpiRamJsonDocument doc( filesize * 2 );

        DeserializationError error = deserializeJson( doc, file );
        if ( error ) {
            log_e("update check deserializeJson() failed: %s", error.c_str() );
        }
        else {
            colorsetup_config.darkon = doc["darkon"] | true;
            colorsetup_config.cprimary = doc["cprimary"] | 0x0000dd;//lv_color_hex(LV_PALETTE_RED);
        }        
        doc.clear();
    }
    file.close();
}
