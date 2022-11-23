#ifndef _UI_SET_STORAGE_H
#define _UI_SET_STORAGE_H

#if __has_include("lvgl.h")
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#define STORAGESETUP_JSON_CONFIG_FILE    "/storage_setup.json"

typedef struct {
    bool    storage_auto = false;
    int32_t storage_brig = 66; 
} storage_config_t;


void openAppStorage();
void AppStorage();
// void storage_save_config( void );
// void storage_load_config( void );
// bool storage_get_storage_auto( void );
// void storage_set_storage_auto( bool storage_auto );
// int32_t storage_get_storage_brig( void );
// void storage_set_storage_brig( int32_t battery_brig );

#endif