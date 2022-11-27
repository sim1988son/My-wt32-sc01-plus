#include <Arduino.h>
#include <string.h>
#include "main.h"
#include "ui/ui.h"
#include "ui/setup/wlan.h"
#include "ui/setup/datetime.h"
#include "ui/setup/display.h"
#include "ui/setup/battery.h"
#include "ui/setup/storage.h"
#include "ui/setup/about.h"
#include "ui/setup/SIM.h"
#include "ui/setup/bluetooth.h"
#include "ui/setup/background.h"
#include "ui/setup/color.h"
#include "ui/setup/sound.h"
#include "ui/setup/lockscreen.h"
#include "ui/setup/language.h"
#include "ui/setup/update.h"
#include "app/weather/weather.h"
#include "app/calculator/calculator.h"
#include "app/alarm/alarm.h"
#include "app/stoper/stoper.h"


lv_obj_t * ui_startScreen;
lv_obj_t * ui_tileView;
// lv_obj_t * ui_displayLabel;
// lv_obj_t * ui_brightnessLabel;
// lv_obj_t * ui_brightnessSlider;

lv_obj_t * ui_statusBar;
lv_obj_t * ui_batteryBar;
lv_obj_t * ui_lockTime;
lv_obj_t * ui_batterysymbol;

lv_obj_t * ui_btnwifi;
lv_obj_t * ui_btnble;
lv_obj_t * ui_btnvol;
lv_obj_t * ui_btnflash;

lv_obj_t * ui_notificationPanel;
lv_obj_t * ui_actionPanel;
lv_obj_t * ui_actionDate;
lv_obj_t * ui_actionBattery;
lv_obj_t * ui_notificationText;
lv_obj_t * ui_dragPanel;
lv_obj_t * ui_Label7;

lv_obj_t * ui_settingsScreen;
lv_obj_t * ui_setPanel;
lv_obj_t * ui_setHeader;
lv_obj_t * ui_setbtn;
lv_obj_t * ui_setIcon;
lv_obj_t * ui_setLabel;
lv_obj_t * list1;

lv_timer_t * ui_statusBar_timer;


static bool statusbar_refresh_update = false;

String ssidNameList, ssidPWList;

typedef struct Drag{
    bool dragging;
    bool active;
    int y;
} drag;

//drag lockscreen;
drag notification;

// Network_Status_t networkStatus = NONE;

lv_status_bar_t statusIcon[ STATUSBAR_NUM ] = {
    {NULL, LV_SYMBOL_CHARGE},
    {NULL, LV_SYMBOL_BELL},
    {NULL, LV_SYMBOL_WIFI},
    {NULL, LV_SYMBOL_BLUETOOTH},
    {NULL, LV_SYMBOL_VOLUME_MAX},
    {NULL, LV_SYMBOL_WARNING},
    {NULL, LV_SYMBOL_SD_CARD},
    {NULL, LV_SYMBOL_EDIT}
};

static void event_navigate(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_disp_t *display = lv_disp_get_default();
    lv_obj_t *actScr = lv_disp_get_scr_act(display);

    if (code == LV_EVENT_CLICKED){
        Serial.println("Navigate action: ");
        if (obj == ui_setbtn){
            openStart();
        }
    }
}

static void ui_event_notificationPanel(lv_event_t *e){
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t *ta = lv_event_get_target(e);
    lv_indev_t *indev = lv_indev_get_act();
    if (event == LV_EVENT_PRESSING){
        if (notification.dragging){
            int vect = indev->proc.types.pointer.act_point.y - notification.y;
            if (notification.active){
                // closing notification panel
                if (vect <= 0 && vect >= -480){
                    lv_obj_set_y(ui_notificationPanel, vect);
                    if (vect >= -320){
                        lv_obj_set_y(ui_actionPanel, 50 + (vect * -1));
                    }
                }
            }else{
                // opening notification panel
                if (vect >= 0 && vect <= 430){
                    lv_obj_set_y(ui_notificationPanel, -430 + vect);
                    if (vect >= 130){
                        lv_obj_set_y(ui_actionPanel, 350 - (vect - 130));
                    }
                }
            }
        }
    }
    if (event == LV_EVENT_PRESSED){
        notification.y = indev->proc.types.pointer.act_point.y;
        if (!notification.active){
            // 0 - 30
            if (notification.y < 50){
                // dragging = true;
                lv_obj_set_style_opa(ui_notificationPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_y(ui_actionPanel, 350);

                lv_obj_set_style_text_opa(ui_actionBattery, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_opa(ui_actionDate, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_opa(ui_notificationText, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            }
        }else{
            lv_obj_set_style_text_opa(ui_actionBattery, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(ui_actionDate, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(ui_notificationText, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        notification.dragging = true;
        // lv_obj_set_y(ui_notificationPanel, -440);
    }
    if (event == LV_EVENT_RELEASED){
        // printf("Release\tx:%d, y:%d\n", indev->proc.types.pointer.act_point.x, indev->proc.types.pointer.act_point.y);
        notification.dragging = false;
        int vect = indev->proc.types.pointer.act_point.y - notification.y;
        if (notification.active){
            if ((vect * -1) < 150){
                lv_obj_set_style_opa(ui_notificationPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_y(ui_notificationPanel, 0);
                lv_obj_set_y(ui_actionPanel, 50);
                lv_obj_set_style_text_opa(ui_actionBattery, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_opa(ui_actionDate, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_opa(ui_notificationText, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                notification.active = true;
            }else{
                lv_obj_set_y(ui_notificationPanel, -430);
                lv_obj_set_style_opa(ui_notificationPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_y(ui_actionPanel, 370);
                lv_obj_set_style_text_opa(ui_actionBattery, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_opa(ui_actionDate, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_opa(ui_notificationText, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                notification.active = false;
            }
        }else{
            if (vect > 150){
                lv_obj_set_style_opa(ui_notificationPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_y(ui_notificationPanel, 0);
                lv_obj_set_y(ui_actionPanel, 50);
                lv_obj_set_style_text_opa(ui_actionBattery, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_opa(ui_actionDate, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_opa(ui_notificationText, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                notification.active = true;
            }else{
                lv_obj_set_y(ui_notificationPanel, -430);
                lv_obj_set_style_opa(ui_notificationPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_y(ui_actionPanel, 370);
                lv_obj_set_style_text_opa(ui_actionBattery, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_opa(ui_actionDate, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_opa(ui_notificationText, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                notification.active = false;
            }
        }
    }
}

static void event_action(lv_event_t *e){
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    int i = (int)lv_event_get_user_data(e);

    if (event == LV_EVENT_VALUE_CHANGED){
        if (lv_obj_has_state(obj, LV_STATE_CHECKED)){
            lv_obj_add_flag(statusIcon[i].icon, LV_OBJ_FLAG_HIDDEN);
        }else{
            lv_obj_clear_flag(statusIcon[i].icon, LV_OBJ_FLAG_HIDDEN);
        }
        statusbar_refresh_update = true;
    }
}

// void checked_statusbar(int i, bool _statusBarIkon){
//     if (_statusBarIkon){
//         lv_obj_clear_flag(statusIcon[i].icon, LV_OBJ_FLAG_HIDDEN);
//     }else{
//         lv_obj_add_flag(statusIcon[i].icon, LV_OBJ_FLAG_HIDDEN);
//     }
//     statusbar_refresh_update = true;
// }

// void checked_sdcard(bool status){
//     checked_statusbar(ui_sdcardIcon, status);
// }

// void checked_wifi(bool status){
//     checked_statusbar(ui_wifiIcon, status);
// }

static void event_launch(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED){
        char buf[150];
        char *data = (char *)lv_event_get_user_data(e);
        // Settings list
        if (data == "Settings"){
            openSettings();
        }
        // Settings list - System
        else if (data == "Display"){
            openAppDisplay();
        }
        else if (data == "Battery"){
            openAppBattery();
        }
        else if (data == "Storage"){
            openAppStorage();
        }
        else if (data == "About"){
            openAppAbout();
        }
        // Settings list - Net 
        else if (data == "SIM"){
            openAppSIM();
        }
        else if (data == "WiFi"){
            openAppwifi();
        }
        else if (data == "Bluetooth"){
            openAppBluetooth();
        }
        // Settings list - Personalization
        else if (data == "Background"){
            openAppBackground();
        }
        else if (data == "Colors"){
            openAppColors();
        }
        else if (data == "Sound"){
            openAppSound();
        }
        else if (data == "Lock screen"){
            openAppLockScreen();
        }
        // Settings list - Apps Setup
        else if (data == "Weather"){
            openAppWeatherSetup();
        }
        else if (data == "Calculator"){
            openAppCalculatorSetup();
        }
        // Settings list - Time
        else if (data == "Date & time"){
            openAppDateTime();
        }
        else if (data == "Language"){
            openAppLanguage();
        }
        // Settings list - Update
        else if (data == "Update"){
            openAppUpdate();
        }
    }
}
    
static void event_handler(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
}

void create_action_tile(lv_obj_t *parent, lv_obj_t *obj2, char *name, const void *src, bool checked, int col, int row, int size, int action, lv_event_cb_t callback){
    lv_obj_t *label;
    lv_obj_t *icon;
    obj2 = lv_btn_create(parent);
    lv_obj_set_style_radius(obj2, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_grid_cell(obj2, LV_GRID_ALIGN_STRETCH, col, size, LV_GRID_ALIGN_STRETCH, row, 1);
    lv_obj_add_flag(obj2, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_event_cb(obj2, callback, LV_EVENT_VALUE_CHANGED , &action);

    if (!checked){
        lv_obj_add_state(obj2, LV_STATE_CHECKED); /*Make the chekbox checked*/
    }else{
        lv_obj_clear_state(obj2, LV_STATE_CHECKED); /*MAke the checkbox unchecked*/
    }
    lv_obj_clear_flag(obj2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(obj2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj2, 20, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(obj2, 255, LV_PART_MAIN | LV_STATE_CHECKED);

    icon = lv_label_create(obj2);
    lv_label_set_text(icon, statusIcon[action].symbol);
    lv_obj_set_align(icon, LV_ALIGN_CENTER);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
}



void add_item(lv_obj_t *parent, char *name, char *src, lv_event_cb_t callback){

    lv_obj_t *obj;
    lv_obj_t *icon;
    lv_obj_t *label;
    lv_obj_t *icon2;

    obj = lv_obj_create(parent);
    // lv_obj_set_width(obj, lv_pct(30));
    lv_obj_set_width(obj, 280);
    lv_obj_set_height(obj, 35);
    lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    //lv_obj_set_style_bg_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_color(obj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    //lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_opa(obj, 250, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(obj, callback, LV_EVENT_ALL, name);

    label = lv_label_create(obj);
    lv_label_set_text(label, name);
    lv_obj_center(label);
    lv_obj_set_align(label, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(label, 30);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    icon = lv_label_create(obj);
    lv_label_set_text(icon, src);
    lv_obj_center(icon);
    lv_obj_set_align(icon, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(icon, 0);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    icon2 = lv_label_create(obj);
    lv_label_set_text(icon2, LV_SYMBOL_RIGHT);
    lv_obj_center(icon2);
    lv_obj_set_align(icon2, LV_ALIGN_RIGHT_MID);
    lv_obj_set_x(icon2, 0);
    lv_obj_set_style_text_font(icon2, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
}

///////////////////// DYNAMIC COMPONENTS ////////////////////

lv_obj_t *create_label(lv_obj_t *parent, uint16_t Pos, const char *text, uint16_t xPos, uint16_t yPos, uint8_t font)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_obj_set_width(label, 280);              /// 1
    // lv_obj_set_height(label, LV_SIZE_CONTENT); /// 1
    if(Pos == 0){lv_obj_set_align(label, LV_ALIGN_TOP_LEFT);}
    else if(Pos == 1){lv_obj_set_align(label, LV_ALIGN_BOTTOM_LEFT);}
    else if(Pos == 2){lv_obj_set_align(label, LV_ALIGN_BOTTOM_RIGHT);}
    else if(Pos == 3){lv_obj_set_align(label, LV_ALIGN_TOP_RIGHT);}
    lv_obj_set_x(label, xPos);
    lv_obj_set_y(label, yPos);
    lv_label_set_text(label, text);
    if(font == 14)lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    if(font == 16)lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    if(font == 20)lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    return label;
}

lv_obj_t *create_slider(lv_obj_t *parent, lv_event_cb_t callback, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height)
{
    lv_obj_t *slider = lv_slider_create(parent);
    lv_slider_set_range(slider, 0, 255);
    lv_obj_set_width(slider, width);
    lv_obj_set_height(slider, height);
    lv_obj_set_x(slider, xPos);
    lv_obj_set_y(slider, yPos);
    lv_obj_set_style_pad_all(slider, 7, LV_PART_KNOB);
    //lv_obj_set_style_x(slider, -14, LV_PART_KNOB);
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_KNOB);
    // lv_obj_set_style_bg_color(slider,lv_color_white(), LV_PART_KNOB);
    lv_obj_add_event_cb(slider, callback, LV_EVENT_VALUE_CHANGED, NULL);
    return slider;
}

lv_obj_t *create_switch(lv_obj_t *parent, lv_event_cb_t callback, uint16_t yPos)
{
    lv_obj_t * Sw = lv_switch_create(parent);
    lv_obj_set_width(Sw, 50);  /// 1
    lv_obj_set_height(Sw, 25); /// 1
    lv_obj_set_x(Sw, -20);
    lv_obj_set_y(Sw, yPos);
    lv_obj_set_align(Sw, LV_ALIGN_TOP_RIGHT);
    lv_obj_add_event_cb(Sw, callback, LV_EVENT_VALUE_CHANGED, NULL);
    return Sw;
}

lv_obj_t *create_label_switch(lv_obj_t *parent, lv_event_cb_t callback, const char *text, uint16_t yPos, uint8_t font)
{
    lv_obj_t * Sw = lv_switch_create(parent);
    lv_obj_set_width(Sw, 50);  /// 1
    lv_obj_set_height(Sw, 25); /// 1
    lv_obj_set_x(Sw, -20);
    lv_obj_set_y(Sw, yPos);
    lv_obj_set_align(Sw, LV_ALIGN_TOP_RIGHT);
    //uint32_t btnwifi_id = 2;
    lv_obj_add_event_cb(Sw, callback, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *label = lv_label_create(parent);
    lv_obj_set_width(label, 230);              /// 1
    lv_obj_set_height(label, LV_SIZE_CONTENT); /// 1
    // lv_obj_set_x(label, 20);
    // lv_obj_set_y(label, yPos);
    lv_obj_align_to(label, Sw, LV_ALIGN_OUT_LEFT_MID,-230,0);
    lv_label_set_text(label, text);
    if(font == 14)lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    if(font == 16)lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    if(font == 20)lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    return Sw;
}

lv_obj_t *create_mbox(lv_obj_t *parent, const char *text, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height)
{
    lv_obj_t * mbox = lv_obj_create(parent);
    lv_obj_clear_flag(mbox, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_width(mbox, width);  /// 1
    lv_obj_set_height(mbox, height); /// 1
    lv_obj_set_x(mbox, xPos);
    lv_obj_set_y(mbox, yPos);
    lv_obj_set_align(mbox, LV_ALIGN_TOP_LEFT);

    lv_obj_t * ui_mboxTitle = lv_label_create(mbox);
    lv_label_set_text(ui_mboxTitle, text);
    lv_obj_align(ui_mboxTitle, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_text_font(ui_mboxTitle, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    return mbox;
}

lv_obj_t *create_mboxbtn(lv_obj_t *parent, lv_event_cb_t callback, bool bbtn)
{
    lv_obj_t * btnLabel;

    lv_obj_t * Btn = lv_btn_create(parent);
    lv_obj_set_width(Btn, 100);  /// 1
    lv_obj_add_event_cb(Btn, callback, LV_EVENT_ALL, NULL);
    if(bbtn){
        lv_obj_align(Btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
        btnLabel = lv_label_create(Btn);
        lv_label_set_text(btnLabel, "OK");
        lv_obj_center(btnLabel);
    }else{
        lv_obj_align(Btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
        btnLabel = lv_label_create(Btn);
        lv_label_set_text(btnLabel, "Cancel");
        lv_obj_center(btnLabel);
    }
    return Btn;
}
lv_obj_t *create_header(lv_obj_t *parent, const char *text)
{
    lv_obj_t * panel = lv_obj_create(parent);
    lv_obj_set_width(panel, 320);
    lv_obj_set_height(panel, 480);
    lv_obj_set_align(panel, LV_ALIGN_TOP_MID);
    lv_obj_set_scrollbar_mode(panel, LV_SCROLLBAR_MODE_ACTIVE);
    lv_obj_set_style_radius(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * header = lv_obj_create(panel);
    lv_obj_set_width(header, 320);
    lv_obj_set_height(header, 70);
    lv_obj_set_align(header, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_radius(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_color(header, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(header, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * label = lv_label_create(header);
    lv_obj_set_width(label, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(label, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(label, 0);
    lv_obj_set_y(label, 0);
    lv_obj_set_align(label, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    return panel;
}

lv_obj_t *create_button(lv_obj_t *parent, lv_event_cb_t callback)
{
    lv_obj_t *button = lv_obj_create(parent);
    lv_obj_set_width(button, 70);  /// 1
    lv_obj_set_height(button, 40); /// 1
    lv_obj_set_x(button, 0);
    lv_obj_set_y(button, 30);
    lv_obj_set_align(button, LV_ALIGN_TOP_LEFT);
    // lv_obj_set_scrollbar_mode(button, LV_SCROLLBAR_MODE_ACTIVE);
    lv_obj_set_style_radius(button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_add_flag(button, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_event_cb(button, callback, LV_EVENT_ALL, NULL);

    lv_obj_t *label = lv_label_create(button);
    lv_obj_set_x(label, 20);
    lv_obj_set_y(label, 0);
    lv_obj_set_align(label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(label, LV_SYMBOL_LEFT);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    return button;
}

lv_obj_t *create_obj(lv_obj_t *parent, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, lv_color_t color)
{
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_set_width(obj, width);  /// 1
    lv_obj_set_height(obj, height); /// 1
    lv_obj_set_x(obj, xPos);
    lv_obj_set_y(obj, yPos);
    lv_obj_set_align(obj, LV_ALIGN_TOP_MID);
    lv_obj_set_style_radius(obj, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, color, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(obj, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    return obj;
}

lv_obj_t *create_obj_color(lv_obj_t *parent, uint16_t xPos, uint16_t yPos, uint16_t w, uint32_t colorp, lv_event_cb_t callback)
{
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_set_width(obj, w);  /// 1
    lv_obj_set_height(obj, w); /// 1
    lv_obj_set_x(obj, xPos);
    lv_obj_set_y(obj, yPos);
    lv_obj_set_align(obj, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_radius(obj, w/2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(colorp), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(obj, callback, LV_EVENT_ALL , NULL);

    // obj = lv_obj_create(obj);
    // lv_obj_set_width(obj, w/2);  /// 1
    // lv_obj_set_height(obj, w/2); /// 1
    // lv_obj_set_align(obj, LV_ALIGN_CENTER);
    // lv_obj_set_style_radius(obj, w/4, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(obj, colors, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    return obj;
}

lv_obj_t *create_roller(lv_obj_t *parent, uint16_t roll, uint16_t xPos, uint16_t yPos)
{
    const char * opts;
    lv_obj_t * roller = lv_roller_create(parent);
    lv_obj_set_width(roller, 70);  /// 1
    lv_obj_set_x(roller, xPos);
    lv_obj_set_y(roller, yPos);
    lv_obj_set_align(roller, LV_ALIGN_CENTER);
    lv_obj_set_style_radius(roller, 0, LV_PART_MAIN | LV_PART_SELECTED);
    lv_obj_set_style_bg_opa(roller, 0, LV_PART_MAIN);
    // lv_obj_set_style_border_color(roller, lv_color_hex(0x323232), LV_PART_MAIN | LV_PART_SELECTED);
    // lv_obj_set_style_outline_color(roller, lv_color_hex(0x323232), LV_PART_MAIN | LV_PART_SELECTED);
    // lv_obj_set_style_shadow_color(roller, lv_color_hex(0x323232), LV_PART_MAIN | LV_PART_SELECTED);
    // lv_obj_set_style_shadow_width(roller, 10, LV_PART_MAIN | LV_PART_SELECTED);
    // lv_obj_set_style_text_color(roller, lv_palette_main(LV_PALETTE_BLUE), LV_PART_SELECTED);
    // lv_obj_set_style_bg_opa(roller, 255, LV_PART_MAIN | LV_PART_SELECTED);
    // lv_obj_set_style_border_width(roller, 0, LV_PART_MAIN | LV_PART_SELECTED);
    lv_obj_set_style_pad_left(roller, 0, LV_PART_MAIN | LV_PART_SELECTED);
    lv_obj_set_style_pad_right(roller, 0, LV_PART_MAIN | LV_PART_SELECTED);
    lv_obj_set_style_pad_top(roller, 0, LV_PART_MAIN | LV_PART_SELECTED);
    lv_obj_set_style_pad_bottom(roller, 0, LV_PART_MAIN | LV_PART_SELECTED);
    lv_obj_set_style_text_font(roller, &lv_font_montserrat_24, LV_PART_MAIN | LV_PART_SELECTED);
    if(roll == 60)opts = "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59";
    if(roll == 24)opts = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24"; //\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25
    if(roll == 28)opts = "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28";
    if(roll == 29)opts = "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29";
    if(roll == 30)opts = "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30";
    if(roll == 31)opts = "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31";
    if(roll == 12)opts = "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12";
    if(roll == 44)opts = "01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31";
    if(roll == 2)opts = "AM\nPM";
    lv_roller_set_options(roller ,opts ,LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(roller, 3);
    return roller;
}

lv_obj_t *create_button_obj(lv_obj_t *parent, lv_event_cb_t callback, uint16_t yPos)
{
    lv_obj_t *button = lv_obj_create(parent);
    lv_obj_set_width(button, 280);  /// 1
    lv_obj_set_height(button, 40); /// 1
    lv_obj_set_x(button, 20);
    lv_obj_set_y(button, yPos);
    lv_obj_clear_flag(button, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_align(button, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_radius(button, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_color(button, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(button, callback, LV_EVENT_ALL, NULL);
    //lv_obj_add_flag(button, LV_OBJ_FLAG_CHECKABLE);

    lv_obj_t *label = lv_label_create(button);
    //lv_obj_set_style_text_color(button, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_x(label, 0);
    lv_obj_set_y(label, 0);
    lv_obj_set_align(label, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(label, LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    return button;
}

void ui_init(void){
    colorsetup_load_config();                                     
    lv_set_theme_color();

    ui_startScreen_screen_init();
    ui_settingsScreen_screen_init();
    lv_disp_load_scr(ui_startScreen);
}

void notification_panel(lv_obj_t *parent){
    // ui_notificationPanel
    ui_notificationPanel = lv_obj_create(parent);

    lv_obj_set_width(ui_notificationPanel, 320);
    lv_obj_set_height(ui_notificationPanel, 480);

    lv_obj_set_x(ui_notificationPanel, 0);
    lv_obj_set_y(ui_notificationPanel, -430);

    lv_obj_set_align(ui_notificationPanel, LV_ALIGN_TOP_MID);

    lv_obj_add_flag(ui_notificationPanel, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_clear_flag(ui_notificationPanel, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_scrollbar_mode(ui_notificationPanel, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_style_radius(ui_notificationPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_notificationPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_notificationPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_notificationPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_notificationPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_notificationPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_notificationPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_notificationPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_notificationPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_notificationPanel, ui_event_notificationPanel, LV_EVENT_ALL, NULL);

    // ui_actionPanel

    ui_actionPanel = lv_obj_create(ui_notificationPanel);

    static lv_coord_t col_dsc[] = {60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {60, 60, 60, LV_GRID_TEMPLATE_LAST};

    lv_obj_set_style_grid_column_dsc_array(ui_actionPanel, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(ui_actionPanel, row_dsc, 0);
    lv_obj_set_width(ui_actionPanel, 320);
    lv_obj_set_height(ui_actionPanel, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_actionPanel, 0);
    lv_obj_set_y(ui_actionPanel, 50);

    lv_obj_set_align(ui_actionPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_actionPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(ui_actionPanel, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_layout(ui_actionPanel, LV_LAYOUT_GRID);

    lv_obj_set_style_radius(ui_actionPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_actionPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_actionPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_actionPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_actionPanel, 16, LV_PART_MAIN | LV_STATE_DEFAULT);

    create_action_tile(ui_actionPanel, ui_btnwifi, "WiFi",      NULL, true, 0, 0, 1, 2, event_action);
    create_action_tile(ui_actionPanel, ui_btnble, "Buetooth",  NULL, true, 1, 0, 1, 3, event_action);
    create_action_tile(ui_actionPanel, ui_btnvol, "Volume",    NULL, false, 2, 0, 1, 4, event_action);
    create_action_tile(ui_actionPanel, ui_btnflash, "Flash",   NULL, false, 3, 0, 1, 7, event_action);

    // ui_actionDate

    ui_actionDate = lv_label_create(ui_notificationPanel);

    lv_obj_set_width(ui_actionDate, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_actionDate, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_actionDate, 11);
    lv_obj_set_y(ui_actionDate, 30);

    lv_obj_set_align(ui_actionDate, LV_ALIGN_TOP_LEFT);

    lv_label_set_text(ui_actionDate, "Czw, 3 lis");

    lv_obj_set_style_text_font(ui_actionDate, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_actionBattery

    ui_actionBattery = lv_label_create(ui_notificationPanel);

    lv_obj_set_width(ui_actionBattery, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_actionBattery, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_actionBattery, -8);
    lv_obj_set_y(ui_actionBattery, 30);

    lv_obj_set_align(ui_actionBattery, LV_ALIGN_TOP_RIGHT);

    lv_label_set_text(ui_actionBattery, "35%");

    lv_obj_set_style_text_font(ui_actionBattery, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_actionBattery, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_actionBattery, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_actionBattery, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_actionBattery, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_notificationText

    ui_notificationText = lv_label_create(ui_notificationPanel);

    lv_obj_set_width(ui_notificationText, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_notificationText, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_notificationText, 10);
    lv_obj_set_y(ui_notificationText, 140);

    lv_label_set_text(ui_notificationText, "No notifications");

    lv_obj_set_style_text_font(ui_notificationText, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_dragPanel

    ui_dragPanel = lv_obj_create(ui_notificationPanel);

    lv_obj_set_width(ui_dragPanel, 320);
    lv_obj_set_height(ui_dragPanel, 10);

    lv_obj_set_x(ui_dragPanel, 0);
    lv_obj_set_y(ui_dragPanel, 0);

    lv_obj_set_align(ui_dragPanel, LV_ALIGN_BOTTOM_MID);

    lv_obj_clear_flag(ui_dragPanel, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_scrollbar_mode(ui_dragPanel, LV_SCROLLBAR_MODE_OFF);

    // lv_obj_add_event_cb(ui_dragPanel, ui_event_dragPanel, LV_EVENT_ALL, NULL);
    lv_obj_set_style_radius(ui_dragPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_dragPanel, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_dragPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_dragPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Label7

    ui_Label7 = lv_label_create(ui_dragPanel);

    lv_obj_set_width(ui_Label7, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label7, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label7, 0);
    lv_obj_set_y(ui_Label7, -5);

    lv_obj_set_align(ui_Label7, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label7, "_____");
}

void status_bar(lv_obj_t *parent){
    // ui_statusBar

    ui_statusBar = lv_obj_create(parent);

    lv_obj_set_width(ui_statusBar, 320);
    lv_obj_set_height(ui_statusBar, 30);

    lv_obj_set_x(ui_statusBar, 0);
    lv_obj_set_y(ui_statusBar, 0);

    lv_obj_set_align(ui_statusBar, LV_ALIGN_TOP_MID);

    lv_obj_clear_flag(ui_statusBar, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_style_radius(ui_statusBar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_color(ui_statusBar, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_statusBar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_statusBar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_statusBar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_statusBar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_statusBar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_statusBar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_batteryBar
    ui_batterysymbol = lv_label_create( ui_statusBar);
    lv_label_set_text( ui_batterysymbol, LV_SYMBOL_BATTERY_FULL);
    lv_obj_align(ui_batterysymbol, LV_ALIGN_TOP_RIGHT, -5, 5);
    lv_obj_set_style_text_font(ui_batterysymbol, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

    for( int i = 0 ; i < STATUSBAR_NUM ; i++ ) {
        statusIcon[i].icon = lv_label_create( ui_statusBar);
        lv_img_set_src( statusIcon[i].icon, statusIcon[i].symbol );
        if (i == 0){
            lv_obj_align_to(statusIcon[i].icon, ui_batterysymbol, LV_ALIGN_OUT_LEFT_MID, -5, 0);
        }else{
            lv_obj_align_to(statusIcon[i].icon, statusIcon[i-1].icon, LV_ALIGN_OUT_LEFT_MID, -5, 0);
        }
    }

    for( int i = 0 ; i < STATUSBAR_NUM ; i++ ) {
        lv_obj_add_flag(statusIcon[i].icon, LV_OBJ_FLAG_HIDDEN); // hide the header
    }
    
    // ui_lockTime

    ui_lockTime = lv_label_create(ui_statusBar);

    lv_obj_set_width(ui_lockTime, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_lockTime, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_lockTime, 5);
    lv_obj_set_y(ui_lockTime, 8);

    lv_obj_set_align(ui_lockTime, LV_ALIGN_TOP_LEFT);

    lv_label_set_text(ui_lockTime, " ");
    // lv_obj_clear_flag(ui_lockTime, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_font(ui_lockTime, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_statusBar_timer = lv_timer_create(ui_statusBar_update_timer, 500,  NULL);
}

void ui_statusBar_update_timer( lv_timer_t  * timer ) {
    if ( statusbar_refresh_update ) {
        statusbar_refresh();
        statusbar_refresh_update = false;
    }
    // String updatelockTime()
    lv_label_set_text(ui_lockTime, updatelockTime().c_str());
}

void statusbar_refresh( void ) {
    for ( int i = 0 ; i < STATUSBAR_NUM ; i++ ) {
        if (i == 0){
            lv_obj_align_to(statusIcon[i].icon, ui_batterysymbol, LV_ALIGN_OUT_LEFT_MID, -5, 0);
        }else{
            if (lv_obj_has_flag(statusIcon[i-1].icon, LV_OBJ_FLAG_HIDDEN)){
                lv_obj_align_to(statusIcon[i].icon, statusIcon[i-1].icon, LV_ALIGN_RIGHT_MID, 0, 0);
            }else{
                lv_obj_align_to(statusIcon[i].icon, statusIcon[i-1].icon, LV_ALIGN_OUT_LEFT_MID, -5, 0);
            }
        }
    }
}

void statusBar_hide_icon(int i){
    lv_obj_add_flag(statusIcon[i].icon, LV_OBJ_FLAG_HIDDEN);
    statusbar_refresh_update = true;
}

void statusBar_show_icon(int i){
    lv_obj_clear_flag(statusIcon[i].icon, LV_OBJ_FLAG_HIDDEN);
    statusbar_refresh_update = true;
}

// lv_obj_t * ui_app_notificationPanel(){
//     return (ui_notificationPanel);
// }

lv_obj_t * ui_app_statusBar(){
    return (ui_statusBar);
}

void create_tile(lv_obj_t *parent, char *name, const void *src, int col, int row, int size, int high, lv_event_cb_t callback){
    lv_obj_t *label;
    lv_obj_t *obj;
    lv_obj_t *icon;
    obj = lv_btn_create(parent);
    lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, col, size, LV_GRID_ALIGN_STRETCH, row, high);
    lv_obj_add_event_cb(obj, callback, LV_EVENT_ALL, name);
    label = lv_label_create(obj);
    lv_label_set_text(label, name);
    lv_obj_center(label);
    lv_obj_set_align(label, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_x(label, -5);
    lv_obj_set_y(label, 5);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    icon = lv_img_create(obj);
    lv_img_set_src(icon, src);
    lv_obj_set_width(icon, LV_SIZE_CONTENT);
    lv_obj_set_height(icon, LV_SIZE_CONTENT);
    lv_obj_set_align(icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(icon, LV_OBJ_FLAG_SCROLLABLE);
}


void openStart(){
    //lv_obj_set_parent(ui_notificationPanel, ui_startScreen);
    lv_obj_set_parent(ui_statusBar, ui_startScreen);
    lv_disp_load_scr(ui_startScreen);
}

void ui_startScreen_screen_init(void){
    // ui_startScreen

    ui_startScreen = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_startScreen, LV_OBJ_FLAG_SCROLLABLE);

    //lv_obj_set_style_bg_color(ui_startScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_startScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // tile view

    ui_tileView = lv_tileview_create(ui_startScreen);

    lv_obj_set_width(ui_tileView, 320);
    lv_obj_set_height(ui_tileView, 480);

    lv_obj_set_x(ui_tileView, 0);
    lv_obj_set_y(ui_tileView, 0);

    lv_obj_set_align(ui_tileView, LV_ALIGN_CENTER);

    lv_obj_set_scrollbar_mode(ui_tileView, LV_SCROLLBAR_MODE_OFF);

    static lv_coord_t col_dsc[] = {67, 67, 67, 67, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {67, 67, 67, 67, 67, 67, 67, LV_GRID_TEMPLATE_LAST};

    /*Create a container with grid*/
    lv_obj_t *cont = lv_obj_create(ui_tileView);
    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
    lv_obj_set_size(cont, 320, 480);
    lv_obj_center(cont);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);

    lv_obj_set_style_pad_top(cont, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_color(cont, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cont, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    create_tile(cont, "Alarm",      LV_SYMBOL_SETTINGS, 0, 0, 2, 2, event_launch);
    create_tile(cont, "Weather",    LV_SYMBOL_SETTINGS, 2, 0, 2, 2, event_launch);

    create_tile(cont, "Messaging",  LV_SYMBOL_SETTINGS, 0, 2, 1, 1, event_launch);
    create_tile(cont, "Store",      LV_SYMBOL_SETTINGS, 1, 2, 1, 1, event_launch);
    create_tile(cont, "Calendar",   LV_SYMBOL_SETTINGS, 2, 2, 1, 1, event_launch);
    create_tile(cont, "Files",      LV_SYMBOL_SETTINGS, 3, 2, 1, 1, event_launch);

    create_tile(cont, "Phone",      LV_SYMBOL_SETTINGS, 0, 3, 1, 1, event_launch);
    create_tile(cont, "Photos",     LV_SYMBOL_SETTINGS, 1, 3, 1, 1, event_launch);
    create_tile(cont, "Settings",   LV_SYMBOL_SETTINGS, 2, 3, 2, 1, event_launch);

    create_tile(cont, "Maps",       LV_SYMBOL_SETTINGS, 0, 4, 1, 1, event_launch);
    create_tile(cont, "OneDrive",   LV_SYMBOL_SETTINGS, 1, 4, 1, 1, event_launch);
    create_tile(cont, "Wallet",     LV_SYMBOL_SETTINGS, 2, 4, 1, 1, event_launch);
    create_tile(cont, "WiFi",       LV_SYMBOL_SETTINGS, 3, 4, 1, 1, event_launch);

    create_tile(cont, "Lock Screen",LV_SYMBOL_SETTINGS, 0, 5, 1, 1, event_launch);
    create_tile(cont, "Sleep",      LV_SYMBOL_SETTINGS, 1, 5, 1, 1, event_launch);
    create_tile(cont, "Calculator", LV_SYMBOL_SETTINGS, 2, 5, 1, 1, event_launch);
    create_tile(cont, "Music",      LV_SYMBOL_SETTINGS, 3, 5, 1, 1, event_launch);
    
    //notification_panel(ui_startScreen);
    status_bar(ui_startScreen);
    statusbar_refresh_update = true;

}

void openSettings(){
    //lv_obj_set_parent(ui_notificationPanel, ui_settingsScreen);
    lv_obj_set_parent(ui_statusBar, ui_settingsScreen);
    lv_disp_load_scr(ui_settingsScreen);
}

void ui_settingsScreen_screen_init(void){
    // ui_startScreen

    ui_settingsScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_settingsScreen, LV_OBJ_FLAG_SCROLLABLE);
    //lv_obj_set_style_bg_color(ui_settingsScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_settingsScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_setHeader = create_header(ui_settingsScreen, "Settings");

    ui_setbtn = create_button(ui_settingsScreen, event_navigate); 

    list1 = lv_list_create(ui_settingsScreen);
    lv_obj_set_size(list1, 320, 410);
    lv_obj_set_x(list1, 0);
    lv_obj_set_y(list1, 70);
    lv_obj_set_align(list1, LV_ALIGN_TOP_MID);

    lv_obj_set_style_pad_top(list1, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(list1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_color(list1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(list1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(list1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_scrollbar_mode(list1, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_top(list1, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(list1, 100, LV_PART_MAIN | LV_STATE_DEFAULT);

    /*Add apps to the list*/
    lv_list_add_text(list1, "System");
    add_item(list1, "Display", LV_SYMBOL_IMAGE, event_launch);
    add_item(list1, "Battery", LV_SYMBOL_BATTERY_EMPTY, event_launch);
    add_item(list1, "Storage", LV_SYMBOL_SD_CARD, event_launch);
    add_item(list1, "About", LV_SYMBOL_HOME, event_launch);

    lv_list_add_text(list1, "Net & wireless");
    add_item(list1, "SIM", LV_SYMBOL_SETTINGS, event_launch);
    add_item(list1, "WiFi", LV_SYMBOL_WIFI, event_launch);
    add_item(list1, "Bluetooth", LV_SYMBOL_BLUETOOTH, event_launch);

    lv_list_add_text(list1, "Personalization");
    add_item(list1, "Background", LV_SYMBOL_IMAGE, event_launch);
    add_item(list1, "Colors", LV_SYMBOL_SETTINGS, event_launch);
    add_item(list1, "Sound", LV_SYMBOL_VOLUME_MAX, event_launch);
    add_item(list1, "Lock screen", LV_SYMBOL_IMAGE, event_launch);
    
    lv_list_add_text(list1, "Apps");
    add_item(list1, "Weather", LV_SYMBOL_SETTINGS, event_launch);
    add_item(list1, "Calculator", LV_SYMBOL_SETTINGS, event_launch);

    lv_list_add_text(list1, "Time & language");
    add_item(list1, "Date & time", LV_SYMBOL_BELL, event_launch);
    add_item(list1, "Language", LV_SYMBOL_SETTINGS, event_launch);

    lv_list_add_text(list1, "Update");
    add_item(list1, "Update", LV_SYMBOL_UPLOAD, event_launch);

    //System
    AppDisplay();
    AppBattery();
    AppStorage();
    AppAbout();
    //Net
    AppSIM();
    Appwifi();
    AppBluetooth();
    //Personan
    AppBackground();
    AppColors();
    AppSound();
    AppLockScreen();
    //Apps
    AppWeatherSetup();
    AppCalculatorSetup();
    //Time
    AppDateTime();
    AppLanguage();
    //update
    AppUpdate();
    
    //notification_panel(ui_settingsScreen);
    status_bar(ui_settingsScreen);
    statusbar_refresh_update = true;

}


