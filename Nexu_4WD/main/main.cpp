
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
#include "driver/gpio.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "sdkconfig.h"
#include <inttypes.h>
#pragma endregion

extern "C"
{
#include "lib/BT/BT.h"
}
#include "lib/config/config.h"
#include 'lib/RS485/RS485.h'

#pragma region Defines

#pragma region Define Pin

#define PinPWMA GPIO_NUM_19
#define PinPWMB GPIO_NUM_17
#define PinPWMC GPIO_NUM_0
#define PinPWMD GPIO_NUM_27

#pragma endregion
#pragma region Define Variables
#define FrequencyMotor 300000
#define DutyResolutionMotor LEDC_TIMER_8_BIT
#pragma endregion

#pragma endregion

#pragma region Variables
int ValueMessage;
int SpeedMessage;
int AngleMessage;
int DirectionMessage;
int Capteur[3];
int VM[4];
bool SendMessage = true;
esp_spp_cb_param_t *param;
int DirectionMotor;

u_int8_t get_message[126];
char Buffer[128];
u_int8_t AddressMotor = 0x44;
u_int8_t AddressSensor = 0x45;
char MotorandSensorBuffer[32];

u_int32_t MotorAndSensorMessage = 0;
uint32_t ValueMotorAndSensor = 0;
uint8_t LengMessage = 0;
uint8_t LengGetMessage = 0;

char DataReceive[126];

int Data = 0;
int length = 0;

#pragma endregion

MotorEncoderHc595 MotorA;
MotorEncoderHc595 MotorB;
MotorEncoderHc595 MotorC;
MotorEncoderHc595 MotorD;

Sensor s1;
Sensor s2;
Sensor s3;


#pragma region Functions Task

void ValueSendMessage(void *pvParameters)
{
    while (1)
    {
        bool EtatSendMessage = SendMessage;
        if (EtatSendMessage == true)
        {
        LengMessage = 5;
#pragma region MsgMotor
            Buffer[0] = AddressMotor;
            for (int i = 0; i < 4; i++)
            {
                Buffer[i+1] = VM[i];
            }
#pragma endregion
        }
        if (EtatSendMessage == false)
        {
        LengMessage = 6;
#pragma region MsgSensor
            Buffer[0] = AddressSensor;
            u_int64_t MessageSensor = MesssageToSensor(Capteur[0], Capteur[1], Capteur[2]);
            //printf("MessageSensor : %llx\n", MessageSensor);
            SendMessageToSensor(MessageSensor);

#pragma endregion
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
void ValueGetMessage(void *pvParameters)
{
    while (1)
    {
        {
            printf("get_message : %x", get_message[0]);
            for (int i = 1; i < LengGetMessage; i++)
            {
                printf(" %x", get_message[i]);
            }
            printf("\n");
            if ((get_message[0] == 0x17) && (LengGetMessage == 4))
            {
                SpeedMessage = get_message[1];
                AngleMessage = get_message[2];
                DirectionMessage = get_message[3];
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
}
static void RS485(void *arg){
    InitRS485();
    s1.SensorAdress(0x12);
    while(1){
        s1.InitSensorTrigger();
        vTaskDelay(70 / portTICK_PERIOD_MS);
        //length = s1.GetDistance();
        length = s1.GetTemp();
    }
}

#pragma endregion

extern "C"
{
    void app_main(void);
}
void app_main(void)
{
    bt_init();
    length = 0;
    xTaskCreate(RS485, "RS485", 2048, NULL, 1, NULL);
    xTaskCreate(ValueSendMessage, "SendMsg", 4096, NULL, 3, NULL);
    xTaskCreate(ValueGetMessage, "GetMsg", 4096, NULL, 2, NULL);
    while (1)
    {
        ESP_LOGI("RS485", " value temp: %f", s1.ValueTemp(DataReceive));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
