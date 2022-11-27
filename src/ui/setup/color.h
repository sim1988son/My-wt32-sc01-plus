#ifndef _UI_SET_COLOR_H
#define _UI_SET_COLOR_H

#if __has_include("lvgl.h")
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#define COLORSETUP_JSON_CONFIG_FILE    "/color_setup.json"

typedef struct {
    bool darkon = true;             /** @brief enable on auto on/off an wakeup and standby */
    int cprimary = 0x0000ff;
} colorsetup_config_t;

void openAppColors();
void AppColors();
void lv_set_theme_color(void);
void colorsetup_save_config( void );
void colorsetup_load_config( void );

#endif