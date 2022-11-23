#ifndef _MAIN_H
#define _MAIN_H

#include <lvgl.h>

#define TFT_RST 4
#define TFT_CS -1
#define TFT_BL 45

#define I2C_SDA 6
#define I2C_SCL 5
#define RST_N_PIN -1
#define INT_N_PIN 7

#define MOTOR -1
#define SCR 30

#define WAKE_PIN GPIO_NUM_7

#define MUSIC_PLAYER 1

#define FORMAT_SPIFFS_IF_FAILED true

#define SD_CS 41
#define SDMMC_CMD 40
#define SDMMC_CLK 39
#define SDMMC_D0 38

#define I2S_DOUT 37
#define I2S_BCLK 36
#define I2S_LRC 35

/* Change to your screen resolution */
static const uint32_t screenWidth = 320;
static const uint32_t screenHeight = 480;

static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;


//char ssid1[20],// ssid2[20], ssid3[20], ssid4[20], ssid5[20];
//char pass1[20],// pass2[20], pass3[20], pass4[20], pass5[20];

void beginWIFITask(void *pvParameters);
void tft_setbrightness(int brig);

//const char *string_data(const char * obj);

#endif