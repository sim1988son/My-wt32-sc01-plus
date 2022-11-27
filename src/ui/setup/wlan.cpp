#include <Arduino.h>
#include "main.h"
#include "ui/ui.h"
#include "ui/setup/wlan.h"
#include "ui/setup/datetime.h"
#include <WiFi.h>
#include <vector>
#include <string.h>
#include "hadware/json_psram_allocator.h"
#include "hadware/alloc.h"
#include "littleFS.h"

bool wifi_init = false;

char * wifiname=NULL;
char * wifipassword=NULL;
const char * wifiobj=NULL;

static networklist * wlansetup_networklist = NULL;
wlansetup_config_t wlansetup_config;

Network_Status_t networkStatus = NONE;

unsigned long networkTimeout = 20 * 1000;
int n=0;
// static int foundNetworks = 0;

// std::vector<String> foundWifiList;
// String ssidName, ssidPW;

TaskHandle_t ntScanTaskHandler, ntConnectTaskHandler;



// Settings list - AppWiFi
lv_obj_t * ui_wifiScreen;
lv_obj_t * ui_wifiHeader;
lv_obj_t * ui_wifibtn;
lv_obj_t * ui_wifilabel;
lv_obj_t * ui_wifiswitch;
lv_obj_t * ui_wifispinner;
lv_obj_t * listwifi;
lv_obj_t * ui_wifimboxlist;
lv_obj_t * ui_wifimboxlistbtnOK;
lv_obj_t * ui_wifimboxlistbtnCL;
lv_obj_t * ui_wifimboxlistbtnTR;
lv_obj_t * ui_wifimboxssid;
lv_obj_t * ui_wifimboxPassword;
lv_timer_t * timer_wifi;
lv_obj_t * ui_systemKeyboard;


void keyboard(lv_obj_t *parent)
{
    // ui_systemKeyboard
    ui_systemKeyboard = lv_keyboard_create(parent);
    lv_obj_clear_flag(ui_systemKeyboard, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_width(ui_systemKeyboard, 320);
    lv_obj_set_height(ui_systemKeyboard, 150);

    lv_obj_set_x(ui_systemKeyboard, 0);
    lv_obj_set_y(ui_systemKeyboard, 0);

    lv_obj_set_align(ui_systemKeyboard, LV_ALIGN_BOTTOM_MID);

    lv_obj_add_flag(ui_systemKeyboard, LV_OBJ_FLAG_HIDDEN);

    lv_obj_set_style_radius(ui_systemKeyboard, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_systemKeyboard, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_systemKeyboard, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_systemKeyboard, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_radius(ui_systemKeyboard, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
}

static void ui_wifiScreen_event(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_disp_t *display = lv_disp_get_default();
    lv_obj_t *actScr = lv_disp_get_scr_act(display);
    //lv_obj_t *ta = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED){
        if (obj == ui_wifibtn){
            wlansetup_save_config();
            openSettings();
        }
    if (obj == ui_wifimboxlistbtnOK) {
        wlansetup_insert_network( wifiobj, lv_textarea_get_text( ui_wifimboxPassword ) );
        networkConnector();
        lv_obj_add_flag(ui_wifimboxlist, LV_OBJ_FLAG_HIDDEN);
    } else if (obj == ui_wifimboxlistbtnCL) {
        lv_obj_add_flag(ui_wifimboxlist, LV_OBJ_FLAG_HIDDEN);
    } else if (obj == ui_wifimboxlistbtnTR){
        wlansetup_delete_network(wifiobj);
    }
    }
    if (code == LV_EVENT_VALUE_CHANGED){
        if (lv_obj_has_state(ui_wifiswitch, LV_STATE_CHECKED)){
            statusBar_show_icon(ui_wifiIcon);
            networkScanner();
            timer_wifi = lv_timer_create(timerForNetwork, 1000, listwifi);
            lv_list_add_text(listwifi, "WiFi: Looking for Networks...");
            lv_obj_clear_flag(ui_wifispinner, LV_OBJ_FLAG_HIDDEN);
            networkStatus = NETWORK_SEARCHING; 
        }else{
            networkStatus = NONE;
            // vTaskDelete(ntScanTaskHandler);
            lv_timer_del(timer_wifi);
            statusBar_hide_icon(ui_wifiIcon);
            lv_obj_clean(listwifi);
            lv_label_set_text(ui_wifilabel, "WiFi");
            if (wifi_status()){
                WiFi.disconnect(true);
            }
        }
        wlan_set_wlansync_auto(lv_obj_has_state(ui_wifiswitch, LV_STATE_CHECKED));
    }
}

static void event_text_input(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *ta = lv_event_get_target(e);

  if (code == LV_EVENT_FOCUSED) {
    lv_obj_move_foreground(ui_systemKeyboard);
    lv_keyboard_set_textarea(ui_systemKeyboard, ta);
    lv_obj_clear_flag(ui_systemKeyboard, LV_OBJ_FLAG_HIDDEN);
  }

  if (code == LV_EVENT_DEFOCUSED) {
    lv_keyboard_set_textarea(ui_systemKeyboard, NULL);
    lv_obj_add_flag(ui_systemKeyboard, LV_OBJ_FLAG_HIDDEN);
  }
}

void openAppwifi(){
    lv_obj_set_parent(ui_app_statusBar(), ui_wifiScreen);
    lv_obj_set_parent(ui_systemKeyboard, ui_wifiScreen);
    lv_disp_load_scr(ui_wifiScreen);
}

void Appwifi(){

    wifi_init = true;

    wlansetup_networklist = (networklist*)CALLOC( sizeof( networklist ) * NETWORKLIST_ENTRYS, 1 );

    if( !wlansetup_networklist ) {
      log_e("wlansetup_networklist calloc faild");
      while(true);
    }

    // clean network list table
    for ( int entry = 0 ; entry < NETWORKLIST_ENTRYS ; entry++ ) {
      wlansetup_networklist[ entry ].ssid[ 0 ] = '\0';
      wlansetup_networklist[ entry ].password[ 0 ] = '\0';
    }

    // load config from spiff
    wlansetup_load_config();

    ui_wifiScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_wifiScreen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(ui_wifiScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_wifiScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_add_event_cb(ui_displayScreen, event_handler, LV_EVENT_SCREEN_UNLOADED, NULL);

    ui_wifiHeader = create_header(ui_wifiScreen, "WiFi");

    ui_wifibtn = create_button(ui_wifiScreen, ui_wifiScreen_event);  

    ui_wifilabel = create_label(ui_wifiScreen,0, "WiFI" , 20, 100, 20);

    ui_wifiswitch = create_switch(ui_wifiScreen, ui_wifiScreen_event, 100);

    /*Create a spinner*/
    ui_wifispinner = lv_spinner_create(ui_wifiScreen, 400, 60);
    lv_obj_add_flag(ui_wifispinner, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(ui_wifispinner, 25, 25);
    lv_obj_set_x(ui_wifispinner, -100);
    lv_obj_set_y(ui_wifispinner, 100);
    lv_obj_set_style_arc_width(ui_wifispinner, 5, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(ui_wifispinner, 5, LV_PART_MAIN);
    // lv_obj_set_style_pad_left(ui_wifispinner, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_pad_right(ui_wifispinner, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_pad_top(ui_wifispinner, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_pad_bottom(ui_wifispinner, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(ui_wifispinner,LV_ALIGN_TOP_RIGHT);

    listwifi = lv_list_create(ui_wifiScreen);
    lv_obj_set_size(listwifi, 320, 350);
    lv_obj_set_x(listwifi, 0);
    lv_obj_set_y(listwifi, 130);
    lv_obj_set_align(listwifi, LV_ALIGN_TOP_MID);

    lv_obj_set_style_pad_top(listwifi, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(listwifi, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_color(listwifi, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(listwifi, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(listwifi, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_scrollbar_mode(listwifi, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_top(listwifi, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(listwifi, 100, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_wifimboxlist = create_mbox(ui_wifiScreen, " ", 20, 130, 280, 240);
    lv_obj_add_flag(ui_wifimboxlist, LV_OBJ_FLAG_HIDDEN);
    ui_wifimboxlistbtnOK = create_mboxbtn(ui_wifimboxlist, ui_wifiScreen_event, true);
    ui_wifimboxlistbtnCL = create_mboxbtn(ui_wifimboxlist, ui_wifiScreen_event, false);
    // ui_wifimboxlistbtnTR = create_button(ui_wifiScreen, ui_wifiScreen_event);  
    
    ui_wifimboxlistbtnTR = lv_btn_create(ui_wifimboxlist);
    lv_obj_set_width(ui_wifimboxlistbtnTR, 40);  /// 1
    lv_obj_set_height(ui_wifimboxlistbtnTR, 40); /// 1
    lv_obj_set_x(ui_wifimboxlistbtnTR, 0);
    lv_obj_set_y(ui_wifimboxlistbtnTR, 0);
    lv_obj_set_align(ui_wifimboxlistbtnTR, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_style_radius(ui_wifimboxlistbtnTR, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_color(ui_wifimboxlistbtnTR, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_wifimboxlistbtnTR, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_wifimboxlistbtnTR, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_wifimboxlistbtnTR, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_wifimboxlistbtnTR, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_wifimboxlistbtnTR, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_wifimboxlistbtnTR, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_wifimboxlistbtnTR, ui_wifiScreen_event, LV_EVENT_ALL, NULL);

    lv_obj_t *label = lv_label_create(ui_wifimboxlistbtnTR);
    lv_obj_set_x(label, 0);
    lv_obj_set_y(label, 0);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, LV_SYMBOL_TRASH);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_wifimboxssid = create_label(ui_wifimboxlist,0, "SSID" , 20, 10, 20);

    ui_wifimboxPassword = lv_textarea_create(ui_wifimboxlist);
    lv_obj_set_size(ui_wifimboxPassword, 240, 40);
    lv_obj_set_x(ui_wifimboxPassword, 0);
    lv_obj_set_y(ui_wifimboxPassword, -10);
    lv_obj_set_align(ui_wifimboxPassword, LV_ALIGN_LEFT_MID);
    lv_textarea_set_placeholder_text(ui_wifimboxPassword, "Password?");
    lv_obj_add_event_cb(ui_wifimboxPassword, event_text_input, LV_EVENT_ALL, NULL); //ui_systemKeyboard

    keyboard(ui_wifiScreen);
    
    // if(wlan_get_wlansync_auto())lv_wifi_on();
}

bool wlan_get_wlansync_auto( void ) {
    return( wlansetup_config.autoon );
}

void wlan_set_wlansync_auto( bool autoon ) {
    wlansetup_config.autoon = autoon;
}

static void timerForNetwork(lv_timer_t *timer) {
  LV_UNUSED(timer);
  switch (networkStatus) {

    case NETWORK_SEARCHING:
      // showingFoundWiFiList();
      // networkStatus = NETWORK_SEARCHING_DONE;
      break;

    case NETWORK_SEARCHING_DONE:
      if(wifi_status()){
        networkStatus = NETWORK_CONNECTED_RUN;
      }else{
        networkConnector();
        lv_obj_add_flag(ui_wifispinner, LV_OBJ_FLAG_HIDDEN);
        networkStatus = NETWORK_CHECK_CONNECTED; 
      }
      break;

    case NETWORK_CHECK_CONNECTED:
      // showingFoundWiFiList();
      // networkStatus = NETWORK_SEARCHING_DONE;
      break;

    case NETWORK_CONNECTED_POPUP:
      networkStatus = NETWORK_CONNECTED_RUN;
      // showingFoundWiFiList();
      break;

    case NETWORK_CONNECTED_RUN:

      // showingFoundWiFiList();
      updateLocalTime();
      break;

    case NETWORK_CONNECT_FAILED:
      networkStatus = NETWORK_SEARCHING;
      // lv_label_set_text(ui_wifilabel, "WiFi - try again");
    //   popupMsgBox("Oops!", "Please check your wifi password and try again.");
      break;

    default:
      break;
  }
}

void networkScanner() {
  xTaskCreate(scanWIFITask,
              "ScanWIFITask",
              16384,
              NULL,
              1,
              &ntScanTaskHandler);
}

static void scanWIFITask(void *pvParameters) {
  while (1) {
    n = WiFi.scanNetworks();
    lv_obj_clean(listwifi);
    lv_list_add_text(listwifi, n > 1 ? "WiFi: Found Networks" : "WiFi: Not Found!");
    vTaskDelay(10);
    for( int i = 0 ; i < n ; i++ ) {
      if ( wlansetup_is_known( WiFi.SSID(i).c_str() ) ) {
        add_wifi_name_list_btn(listwifi, WiFi.SSID(i).c_str(), LV_SYMBOL_EYE_OPEN);
      }
      else {
        add_wifi_name_list_btn(listwifi, WiFi.SSID(i).c_str(), LV_SYMBOL_EYE_CLOSE);
      }
    }
    networkStatus = NETWORK_SEARCHING_DONE; 
    vTaskDelete(NULL);
  }
}

void networkConnector() {
  xTaskCreate(beginWIFITask,
              "beginWIFITask",
              16384,
              NULL,
              1,
              &ntConnectTaskHandler);
}

void beginWIFITask(void *pvParameters) {
// void networkConnector() {
  unsigned long startingTime = millis();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  vTaskDelay(100);

  int len = WiFi.scanComplete();
  for( int i = 0 ; i < len ; i++ ) {
    for ( int entry = 0 ; entry < NETWORKLIST_ENTRYS ; entry++ ) {
      if ( !strcmp( wlansetup_networklist[ entry ].ssid,  WiFi.SSID(i).c_str() ) ) {
        wifiname = wlansetup_networklist[ entry ].ssid;
        wifipassword = wlansetup_networklist[ entry ].password;
        // wifictl_send_event_cb( WIFICTL_SCAN, (void *)"connecting ..." );
        WiFi.begin( wifiname, wifipassword );
        while (WiFi.status() != WL_CONNECTED && (millis() - startingTime) < networkTimeout) {
          vTaskDelay(250);
        }
        if (WiFi.status() == WL_CONNECTED) {
          lv_label_set_text_fmt(ui_wifilabel, "WiFi Y-%s", wifiname);
          networkStatus = NETWORK_CONNECTED_POPUP;
        } else {
          networkStatus = NETWORK_CONNECT_FAILED;
          lv_label_set_text_fmt(ui_wifilabel, "WiFi N-%s", wifiname);
        }
        // return;

      }
    }
  }
  configlockaltime();
  vTaskDelete(NULL);
}

void showingFoundWiFiList(){
  //int n = WiFi.scanComplete();
  if(n > 1){
    lv_obj_clean(listwifi);
    lv_list_add_text(listwifi, "WiFi: Connect Networks");
    add_wifi_name_list_btn(listwifi, wifiname, LV_SYMBOL_EYE_OPEN);
    lv_list_add_text(listwifi, "WiFi: Found Networks");
    for( int i = 0 ; i < n ; i++ ) {
      if ( wlansetup_is_known( WiFi.SSID(i).c_str() ) ) {
        add_wifi_name_list_btn(listwifi, WiFi.SSID(i).c_str(), LV_SYMBOL_EYE_OPEN);
      }
      else {
        add_wifi_name_list_btn(listwifi, WiFi.SSID(i).c_str(), LV_SYMBOL_EYE_CLOSE);
      }
    }
  }
}

bool wifi_status(){
  return (WiFi.status() == WL_CONNECTED);
}

void lv_wifi_on(void){
    lv_obj_add_state(ui_wifiswitch, LV_STATE_CHECKED);
    lv_event_send(ui_wifiswitch, LV_EVENT_VALUE_CHANGED, NULL);
}

void event_mbox(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    
    if (code == LV_EVENT_CLICKED){
        wifiobj =  lv_list_get_btn_text(listwifi, obj); 
        lv_label_set_text(ui_wifimboxssid, wifiobj);    
        lv_obj_clear_flag(ui_wifimboxlist, LV_OBJ_FLAG_HIDDEN);
    }
}


void add_wifi_name_list_btn(lv_obj_t *parent, const char *name, const char *src)
{
    lv_obj_t *obj;
    lv_obj_t *icon;
    lv_obj_t *label;

    obj = lv_obj_create(parent);
    // lv_obj_set_width(obj, lv_pct(100));
    lv_obj_set_width(obj,280);  /// 1
    lv_obj_set_height(obj, 40); /// 1
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    // lv_obj_set_style_bg_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(obj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(obj, 50, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(obj, event_mbox, LV_EVENT_ALL, &name);

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

}

bool wlansetup_is_known( const char* networkname ) {
  if ( wifi_init == false )
    return( false );

  for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
    if( !strcmp( networkname, wlansetup_networklist[ entry ].ssid ) ) {
      return( true );
    }
  }
  return( false );
}


bool wlansetup_delete_network( const char *ssid ) {
  if ( wifi_init == false )
    return( false );

  for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
    if( !strcmp( ssid, wlansetup_networklist[ entry ].ssid ) ) {
      wlansetup_networklist[ entry ].ssid[ 0 ] = '\0';
      wlansetup_networklist[ entry ].password[ 0 ] = '\0';
      wlansetup_save_config();
      return( true );
    }
  }
  return( false );
}

// void connect_btn_pass(const char * btnPass, const char * listSsid){
//   // ssidPW = String(btnPass);
//   // ssidName = String(listSsid);
// }

bool wlansetup_insert_network( const char *ssid, const char *password ) {
  if ( wifi_init == false )
    return( false );

  // check if existin
  for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
    if( !strcmp( ssid, wlansetup_networklist[ entry ].ssid ) ) {
      strlcpy( wlansetup_networklist[ entry ].password, password, sizeof( wlansetup_networklist[ entry ].password ) );
      wlansetup_save_config();
      WiFi.scanNetworks();
      // wifictl_set_event( WIFICTL_SCAN );
      return( true );
    }
  }
  // check for an emty entry
  for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
    if( strlen( wlansetup_networklist[ entry ].ssid ) == 0 ) {
      strlcpy( wlansetup_networklist[ entry ].ssid, ssid, sizeof( wlansetup_networklist[ entry ].ssid ) );
      strlcpy( wlansetup_networklist[ entry ].password, password, sizeof( wlansetup_networklist[ entry ].password ) );
      wlansetup_save_config();
      WiFi.scanNetworks();
      // wifictl_set_event( WIFICTL_SCAN );
      return( true );
    }
  }
  return( false ); 
}

void wlansetup_save_config( void ) {
    fs::File file = LittleFS.open( WLANSETUP_JSON_CONFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", WLANSETUP_JSON_CONFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 10000 );

        doc["autoon"] = wlansetup_config.autoon;

        // doc["enable_on_standby"] = wlansetup_config.enable_on_standby;
        for ( int i = 0 ; i < NETWORKLIST_ENTRYS ; i++ ) {
            doc["networklist"][ i ]["ssid"] = wlansetup_networklist[ i ].ssid;
            doc["networklist"][ i ]["psk"] = wlansetup_networklist[ i ].password;
        }

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

void wlansetup_load_config( void ) {
    fs::File file = LittleFS.open( WLANSETUP_JSON_CONFIG_FILE, FILE_READ );
    if (!file) {
        log_e("Can't open file: %s!", WLANSETUP_JSON_CONFIG_FILE );
    }
    else {
        int filesize = file.size();
        SpiRamJsonDocument doc( filesize * 2 );

        DeserializationError error = deserializeJson( doc, file );
        if ( error ) {
            log_e("update check deserializeJson() failed: %s", error.c_str() );
        }
        else {
            wlansetup_config.autoon = doc["autoon"] | true;

            // wlansetup_config.enable_on_standby = doc["enable_on_standby"] | false;
            for ( int i = 0 ; i < NETWORKLIST_ENTRYS ; i++ ) {
                if ( doc["networklist"][ i ]["ssid"] && doc["networklist"][ i ]["psk"] ) {
                    strlcpy( wlansetup_networklist[ i ].ssid    , doc["networklist"][ i ]["ssid"], sizeof( wlansetup_networklist[ i ].ssid ) );
                    strlcpy( wlansetup_networklist[ i ].password, doc["networklist"][ i ]["psk"], sizeof( wlansetup_networklist[ i ].password ) );
                }
            }
        }        
        doc.clear();
    }
    file.close();
}


