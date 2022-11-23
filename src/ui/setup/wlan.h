#ifndef _UI_SET_WLAN_H
#define _UI_SET_WLAN_H

#if __has_include("lvgl.h")
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


typedef enum {
        NONE,
        NETWORK_SEARCHING,
        NETWORK_SEARCHING_DONE,
        NETWORK_CHECK_CONNECTED,
        NETWORK_CONNECTED_POPUP,
        NETWORK_CONNECTED_RUN,
        NETWORK_CONNECT_FAILED
    } Network_Status_t;


#define WLANSETUP_DELAY               10
#define NETWORKLIST_ENTRYS            20
#define WLANSETUP_JSON_CONFIG_FILE    "/wlan_setup.json"

typedef struct {
    char ssid[64]="";
    char password[64]="";
} networklist;

typedef struct {
    bool autoon = true;             /** @brief enable on auto on/off an wakeup and standby */

} wlansetup_config_t;



void wlansetup_setup( void );
bool wlansetup_is_known( const char* networkname );
bool wlansetup_delete_network( const char *ssid );
bool wlansetup_insert_network( const char *ssid, const char *password );
void wlansetup_save_config( void ) ;
void wlansetup_load_config( void );
bool wlan_get_wlansync_auto( void );
void wlan_set_wlansync_auto( bool autoon );

void openAppwifi();
void Appwifi();

void connect_btn_pass(const char * btnPass, const char * listSsid);
void add_wifi_name_list_btn(lv_obj_t *parent, const char *name, const char *src);
void add_wifi_name_list(const char * nameWifi);
static void scanWIFITask(void *pvParameters);

void task_scan_delete(void);
void networkScanner(void);
void networkConnector(void); 
bool wifi_status(void);
void wifi_disconnect(void);
int showScanNetworks(void);
void showingFoundWiFiList(void);
int viewfoundNetworks();
void cleanWifi(void);
void lv_wifi_on(void);
void network_status_CONNECT_FAILED();
void network_status_CONNECT_POPUP(const char *name);

#endif

