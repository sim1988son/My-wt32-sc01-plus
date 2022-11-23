#ifndef _UI_H
#define _UI_H

#if __has_include("lvgl.h")
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


//char *ssidwifi ;
//String i_Wifi;
static char ssidwifi[64];

#define STATUSBAR_NUM  8

typedef struct {
        lv_obj_t *icon;
        const char *symbol;
    } lv_status_bar_t;

typedef enum {
        ui_chargeIcon = 0,
        ui_alarmIcon ,
        ui_wifiIcon,
        ui_bleIcon,
        ui_volumeIcon,
        ui_waringIcon,
        ui_sdcardIcon,
        ui_flashIcon

    } statusbar_icon_t;




void ui_init(void);

void ui_startScreen_screen_init(void);
void ui_settingsScreen_screen_init(void);
void ui_appScreen_screen_init(void);
void ui_wifiScreen_screen_init(void);
//void buildPWMsgBox(void);

void openStart(void);
void openSettings(void);

void openAppWifi(void);

void openAppFiles(void);

// void openAppDateTime(void);
// void AppDateTime();

void checked_statusbar(int i, bool _statusBarIkon);
void checked_sdcard(bool status);
void checked_wifi(bool status);

void statusbar_refresh(void);

static void timerForNetwork(lv_timer_t *timer);
void ui_statusBar_update_timer( lv_timer_t  * timer );

void add_item(lv_obj_t *parent, char *name, char *src, lv_event_cb_t callback);

// void headerApp(const char *name, const char *name2, bool header);
// void closeApp(void);

lv_obj_t *create_header(lv_obj_t *parent, const char *text);
lv_obj_t *create_button(lv_obj_t *parent, lv_event_cb_t callback);
lv_obj_t *create_label(lv_obj_t *parent, uint16_t Pos, const char *text, uint16_t xPos, uint16_t yPos,  uint8_t font);
lv_obj_t *create_switch(lv_obj_t *parent, lv_event_cb_t callback, uint16_t yPos);
lv_obj_t *create_label_switch(lv_obj_t *parent, lv_event_cb_t callback, const char *text, uint16_t yPos, uint8_t font);
lv_obj_t *create_slider(lv_obj_t *parent, lv_event_cb_t callback, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height);
lv_obj_t *create_mbox(lv_obj_t *parent, const char *text, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height);
lv_obj_t *create_mboxbtn(lv_obj_t *parent, lv_event_cb_t callback, bool bbtn);
lv_obj_t *create_button_obj(lv_obj_t *parent, lv_event_cb_t callback, uint16_t yPos);
lv_obj_t *create_roller(lv_obj_t *parent, uint16_t roll, uint16_t xPos, uint16_t yPos);

// lv_obj_t * ui_app_notificationPanel();
lv_obj_t * ui_app_statusBar();
// lv_obj_t * app_canvas();
#endif
