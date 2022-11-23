#ifndef _UI_SET_DISPLAY_H
#define _UI_SET_DISPLAY_H

#if __has_include("lvgl.h")
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


#define DISPLAYSETUP_JSON_CONFIG_FILE    "/display_setup.json"

typedef struct {
    bool    display_auto = false;
    int32_t display_brig = 66; 
} display_config_t;


void openAppDisplay();
void AppDisplay();
void display_save_config( void );
void display_load_config( void );
bool display_get_display_auto( void );
void display_set_display_auto( bool display_auto );
int32_t display_get_display_brig( void );
void display_set_display_brig( int32_t display_brig );


#endif