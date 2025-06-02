#ifndef BT_h
#define BT_H

#pragma region Includes
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "time.h"
#include "sys/time.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "sdkconfig.h"
#pragma endregion


extern u_int8_t get_message[16];
extern uint8_t LengGetMessage;
extern int VM[4];
extern int Capteur[3];
extern char Buffer[16];
extern bool SendMessage;
extern uint8_t LengMessage;

#pragma region Defines
void bt_init();
static char *bda2str(uint8_t *bda, char *str, size_t size);
static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

#endif