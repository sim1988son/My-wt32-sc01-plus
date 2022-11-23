#ifndef _UI_SET_BATTERY_H
#define _UI_SET_BATTERY_H

#if __has_include("lvgl.h")
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


#define BATTERYSETUP_JSON_CONFIG_FILE    "/battery_setup.json"

typedef struct {
    bool    battery_auto = false;
    int32_t battery_brig = 66; 
} battery_config_t;


void openAppBattery();
void AppBattery();
void battery_save_config( void );
void battery_load_config( void );
bool battery_get_battery_auto( void );
void battery_set_battery_auto( bool battery_auto );
int32_t battery_get_battery_brig( void );
void battery_set_battery_brig( int32_t battery_brig );


#endif