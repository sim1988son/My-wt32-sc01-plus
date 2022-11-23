#ifndef _UI_SET_DATETIME_H
#define _UI_SET_DATETIME_H

#if __has_include("lvgl.h")
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#define DATETIMESETUP_JSON_CONFIG_FILE    "/datetime_setup.json"

typedef struct {
    bool timesync_auto = true;
    bool timesync_24hr = true;             /** @brief enable on auto on/off an wakeup and standby */
    int32_t timesync_GMT = 1; 
} datetime_config_t;


void openAppDateTime();
void AppDateTime();
void updateLocalTime();
void configlockaltime();
String updatelockTime();

bool datetime_get_timesync_auto( void );
void datetime_set_timesync_auto( bool timesync_auto );
bool datetime_get_timesync_24hr( void );
void datetime_set_timesync_24hr( bool timesync_24hr );
int32_t datetime_get_timesync_GMT( void );
void datetime_set_timesync_GMT( int32_t timesync_GMT );

void datetime_save_config( void );
void datetime_load_config( void );

#endif