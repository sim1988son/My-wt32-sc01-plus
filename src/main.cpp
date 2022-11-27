#define LGFX_USE_V1
#define TWATCH_USE_PSRAM_ALLOC_LVGL

#include <Arduino.h>
#include "main.h"
#include <lvgl.h>
#include "ui/ui.h"
#include "ui/setup/wlan.h"
// #include "ui/setup/datetime.h"
// #include "ui/setup/display.h"
// #include "ui/setup/battery.h"
// #include "ui/setup/storage.h"
// #include "ui/setup/about.h"

#include <ESP32Time.h>
#include <WiFi.h>
#include "SD.h"
#include "LittleFS.h"
#include "Audio.h"
#include <vector>
#include <string.h>

#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device{
  lgfx::Panel_ST7796 _panel_instance;
  lgfx::Bus_Parallel8 _bus_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Touch_FT5x06 _touch_instance;

public:
  LGFX(void){
    {
      auto cfg = _bus_instance.config();

      //cfg.port = 0;
      cfg.freq_write = 40000000;
      cfg.pin_wr = 47; // pin number connecting WR
      cfg.pin_rd = -1; // pin number connecting RD
      cfg.pin_rs = 0;  // Pin number connecting RS(D/C)
      cfg.pin_d0 = 9;  // pin number connecting D0
      cfg.pin_d1 = 46; // pin number connecting D1
      cfg.pin_d2 = 3;  // pin number connecting D2
      cfg.pin_d3 = 8;  // pin number connecting D3
      cfg.pin_d4 = 18; // pin number connecting D4
      cfg.pin_d5 = 17; // pin number connecting D5
      cfg.pin_d6 = 16; // pin number connecting D6
      cfg.pin_d7 = 15; // pin number connecting D7

      _bus_instance.config(cfg);              // Apply the settings to the bus.
      _panel_instance.setBus(&_bus_instance); // Sets the bus to the panel.
    }
    {                                      // Set display panel control.
      auto cfg = _panel_instance.config(); // Get the structure for display panel settings.

      cfg.pin_cs = -1;   // Pin number to which CS is connected (-1 = disable)
      cfg.pin_rst = 4;   // pin number where RST is connected (-1 = disable)
      cfg.pin_busy = -1; // pin number to which BUSY is connected (-1 = disable)

      // * The following setting values ​​are set to general default values ​​for each panel, and the pin number (-1 = disable) to which BUSY is connected, so please try commenting out any unknown items.

      cfg.memory_width = 320;  // Maximum width supported by driver IC
      cfg.memory_height = 480; // Maximum height supported by driver IC
      cfg.panel_width = 320;   // actual displayable width
      cfg.panel_height = 480;  // actual displayable height
      cfg.offset_x = 0;        // Panel offset in X direction
      cfg.offset_y = 0;        // Panel offset in Y direction
      cfg.offset_rotation = 2;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = false;
      cfg.invert = true;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;

      _panel_instance.config(cfg);
    }
    {                                      // Set backlight control. (delete if not necessary)
      auto cfg = _light_instance.config(); // Get the structure for backlight configuration.

      cfg.pin_bl = 45;     // pin number to which the backlight is connected
      cfg.invert = false;  // true to invert backlight brightness
      cfg.freq = 44100;    // backlight PWM frequency
      cfg.pwm_channel = 0; // PWM channel number to use

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance); // Sets the backlight to the panel.
    }
    { // Configure settings for touch screen control. (delete if not necessary)
      auto cfg = _touch_instance.config();

      cfg.x_min = 0;   // Minimum X value (raw value) obtained from the touchscreen
      cfg.x_max = 319; // Maximum X value (raw value) obtained from the touchscreen
      cfg.y_min = 0;   // Minimum Y value obtained from touchscreen (raw value)
      cfg.y_max = 479; // Maximum Y value (raw value) obtained from the touchscreen
      cfg.pin_int = 7; // pin number to which INT is connected
      cfg.bus_shared = false;
      cfg.offset_rotation = 0;

      // For I2C connection
      cfg.i2c_port = 0;    // Select I2C to use (0 or 1)
      cfg.i2c_addr = 0x38; // I2C device address number
      cfg.pin_sda = 6;     // pin number where SDA is connected
      cfg.pin_scl = 5;     // pin number to which SCL is connected
      cfg.freq = 400000;   // set I2C clock

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance); // Set the touchscreen to the panel.
    }
    setPanel(&_panel_instance); // Sets the panel to use.
  }
};

LGFX tft;

void saveWIFICredentialEEPROM(int flag, String ssidpw);
void loadWIFICredentialEEPROM();

static lv_color_t disp_draw_buf[screenWidth * SCR];
static lv_color_t disp_draw_buf2[screenWidth * SCR];

long currentMillis;

int brightness=66;


Audio audio;

bool playing;

// Display callback to flush the buffer to screen
void display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p){
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushPixels((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

// Touchpad callback to read the touchpad
void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data){
    uint16_t touchX, touchY;
    bool touched = tft.getTouch(&touchX, &touchY);

    if (!touched){
        data->state = LV_INDEV_STATE_REL;
    }else{
        data->state = LV_INDEV_STATE_PR;
        // Set the coordinates
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

void tft_setbrightness(int brig){
  tft.setBrightness(brig);
}

#ifdef LVGL_LOOP
void lvgl_loop(void *parameter){
  while (true)
  {
    lv_timer_handler();
    delay(5);
  }
  vTaskDelete(NULL);
}

void guiHandler(){
  xTaskCreatePinnedToCore(
      // xTaskCreate(
      lvgl_loop,   // Function that should be called
      "LVGL Loop", // Name of the task (for debugging)
      16384,       // Stack size (bytes)
      NULL,        // Parameter to pass
      1,           // Task priority
      // NULL
      NULL, // Task handle
      1);
}
#endif

void wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup caused by ULP program");
    break;
  default:
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
    // rtc.setTime(1663491331); // Sunday, September 18, 2022 8:55:31 AM
    break;
  }
}

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
 
  //AUDIO
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  //audio.setVolume(10); // 0...21
  audio.forceMono(true);

  tft.init();        // Initialize LovyanGFX
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);

  //SD
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SPI.begin(SDMMC_CLK, SDMMC_D0, SDMMC_CMD);
  bool statusSdCard = SD.begin(SD_CS);

  
  if (!LittleFS.begin(FORMAT_SPIFFS_IF_FAILED))
  {
    Serial.println("LittleFS Mount Failed");
    //tft.setBrightness(brightness);

    delay(2000);
    ESP.restart();
  }

  //wakeup_reason();

  lv_init();         // Initialize lvgl

    /* LVGL : Setting up buffer to use for display */
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, disp_draw_buf2, screenWidth * SCR);

    /*** LVGL : Setup & Initialize the display device driver ***/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = display_flush;
    disp_drv.draw_buf = &draw_buf;
    //disp_drv.sw_rotate = 1;
    //disp = 
    lv_disp_drv_register(&disp_drv);

    //*** LVGL : Setup & Initialize the input device driver ***
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    lv_indev_drv_register(&indev_drv);  

  ui_init();
  openStart();
  //checked_statusbar(ui_sdcardIcon, statusSd);
  // checked_sdcard(statusSdCard);
  // checked_wifi(false);
  if(statusSdCard)statusBar_show_icon(ui_sdcardIcon);
  else statusBar_hide_icon(ui_sdcardIcon);

#ifdef LVGL_LOOP
  guiHandler();
#endif
  //tft_setbrightness(33);
  // tft.setBrightness(66);
  currentMillis = millis();
  while (currentMillis + 2000 > millis())
  {
#ifndef LVGL_LOOP
    lv_timer_handler();
#endif
  }

  currentMillis = millis();

  if(wlan_get_wlansync_auto())lv_wifi_on();

}

void loop() {

  lv_timer_handler(); /* let the GUI do its work */

}