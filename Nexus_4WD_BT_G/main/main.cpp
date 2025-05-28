
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
#include "esp32/rom/ets_sys.h"
#pragma endregion

extern "C"
{
#include "lib/BT/BT.h"
}
#include "rhoban_shell.h"
#include "lib/config/config.h"
#include "lib/RS485/RS485.h"

#pragma region Defines

#pragma region Define Pin

#define PinPWMA GPIO_NUM_19
#define PinMAEncoderA GPIO_NUM_34
#define PinMAEncoderB GPIO_NUM_35
#define PinPWMB GPIO_NUM_18
#define PinMBEncoderA GPIO_NUM_32
#define PinMBEncoderB GPIO_NUM_33
#define PinPWMC GPIO_NUM_5
#define PinMCEncoderA GPIO_NUM_25
#define PinMCEncoderB GPIO_NUM_26
#define PinPWMD GPIO_NUM_17
#define PinMDEncoderA GPIO_NUM_27
#define PinMDEncoderB GPIO_NUM_14

#define DS GPIO_NUM_16
#define STCP GPIO_NUM_4
#define SHCP GPIO_NUM_0

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
int SM[4];
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
int OldData = 0;
int length = 0;

#pragma endregion

SHELL_PARAMETER_FLOAT(kp, "kp", 1);
SHELL_PARAMETER_FLOAT(ki, "ki", 2.0);
SHELL_PARAMETER_FLOAT(kd, "kd", 0.001);
SHELL_PARAMETER_INT(co, "co", 0);
SHELL_PARAMETER_INT(etat, "etat", 0);

MotorEncoderHc595 MotorA;
MotorEncoderHc595 MotorB;
MotorEncoderHc595 MotorC;
MotorEncoderHc595 MotorD;

Sensor s1;
Sensor s2;
Sensor s3;

#pragma region functions

u_int64_t MesssageToSensor(int16_t Sensor1, int16_t Sensor2, int16_t Sensor3)
{
    u_int64_t Message = 0;
    Message |= ((u_int64_t)(Sensor1 & 0xFFF) << 24);
    Message |= ((u_int64_t)(Sensor2 & 0xFFF) << 12);
    Message |= (u_int64_t)(Sensor3 & 0xFFF);
    return Message;
}

void SendMessageToSensor(u_int64_t Message)
{
    for (int i = 0; i < 5; i++)
    {
        Buffer[i + 1] = (Message >> ((8 * 4) - (8 * i))) & 0xFF;
    }
}

#pragma endregion

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
                Buffer[i + 1] = SM[i] * 255 / 7300;
            }
#pragma endregion
        }
        if (EtatSendMessage == false)
        {
            LengMessage = 6;
#pragma region MsgSensor
            Buffer[0] = AddressSensor;
            u_int64_t MessageSensor = MesssageToSensor(Capteur[0], Capteur[1], Capteur[2]);
            // ESP_LOGI("Message", "MEssage = %llx",MessageSensor );
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
static void RS485(void *arg)
{
    InitRS485();
    while (1)
    {
        for (int i = 0; i < 3; i++)
        {
            switch (i)
            {
            case 0:
                s1.InitSensorTrigger();
                vTaskDelay(80 / portTICK_PERIOD_MS);
                length = s1.GetDistance();
                break;
            case 1:
                s2.InitSensorTrigger();
                vTaskDelay(80 / portTICK_PERIOD_MS);
                length = s2.GetDistance();
                break;
            case 2:
                s3.InitSensorTrigger();
                vTaskDelay(80 / portTICK_PERIOD_MS);
                length = s3.GetDistance();
                break;
            }
        }
        // length = s1.GetTemp();
    }
}
void SetSpeeds(void *arg)
{
    while (1)
    {
        float Consigne = SpeedMessage * 7500.0 / 255.0;
        MotorA.SetSpeedPID(Consigne, SM[0], kp, ki, kd);
        MotorB.SetSpeedPID(Consigne, SM[1], kp, ki, kd);
        MotorC.SetSpeedPID(Consigne, SM[2], kp, ki, kd);
        MotorD.SetSpeedPID(Consigne, SM[3], kp, ki, kd);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
void SetAngle(void *arg)
{
    while (1)
    {
        uint16_t GetAngle = (AngleMessage * 360) / 255;
        // printf("GetAngle : %d\n", GetAngle);
        if (GetAngle < 180)
        {
            Data = MotorA.DirHc595(1);
            Data = MotorB.DirHc595(1);
            Data = MotorC.DirHc595(1);
            Data = MotorD.DirHc595(1);
        }
        else
        {
            Data = MotorA.DirHc595(2);
            Data = MotorB.DirHc595(2);
            Data = MotorC.DirHc595(2);
            Data = MotorD.DirHc595(2);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
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
    shell_init(115200);
    shell_start_task();
    s1.SensorAdress(0x11);
    s2.SensorAdress(0x12);
    s3.SensorAdress(0x13);
#pragma region Init Motor
#pragma region Init Motor A
    MotorA.MotorAttached(PinPWMA, LEDC_CHANNEL_0, 0);
    MotorA.MotorResolution(FrequencyMotor, DutyResolutionMotor);
    MotorA.EncodeurAttached(PinMAEncoderA, PinMAEncoderB, PCNT_UNIT_0, 10000);
    MotorA.hc595Attached(DS, SHCP, STCP);
    MotorA.InitMotorEncodeurHC595();
#pragma endregion
#pragma region Init Motor B
    MotorB.MotorAttached(PinPWMB, LEDC_CHANNEL_1, 1);
    MotorB.MotorResolution(FrequencyMotor, DutyResolutionMotor);
    MotorB.EncodeurAttached(PinMBEncoderA, PinMBEncoderB, PCNT_UNIT_1, 10000);
    MotorB.hc595Attached(DS, SHCP, STCP);
    MotorB.InitMotorEncodeurHC595();
#pragma endregion
#pragma region Init Motor C
    MotorC.MotorAttached(PinPWMC, LEDC_CHANNEL_2, 2);
    MotorC.MotorResolution(FrequencyMotor, DutyResolutionMotor);
    MotorC.EncodeurAttached(PinMCEncoderA, PinMCEncoderB, PCNT_UNIT_2, 10000);
    MotorC.hc595Attached(DS, SHCP, STCP);
    MotorC.InitMotorEncodeurHC595();
#pragma endregion
#pragma region Init Motor D
    MotorD.MotorAttached(PinPWMD, LEDC_CHANNEL_3, 3);
    MotorD.MotorResolution(FrequencyMotor, DutyResolutionMotor);
    MotorD.EncodeurAttached(PinMDEncoderA, PinMDEncoderB, PCNT_UNIT_3, 10000);
    MotorD.hc595Attached(DS, SHCP, STCP);
    MotorD.InitMotorEncodeurHC595();
#pragma endregion
#pragma endregion
    length = 0;
    xTaskCreate(SetAngle, "SetAngle", 2048, NULL, 0, NULL);
    xTaskCreate(SetSpeeds, "SetSpeed", 2048, NULL, 4, NULL);
    xTaskCreate(RS485, "RS485", 2048, NULL, 2, NULL);
    xTaskCreate(ValueGetMessage, "GetMsg", 4096, NULL, 5, NULL);
    xTaskCreate(ValueSendMessage, "SendMsg", 4096, NULL, 3, NULL);
    while (1)
    {
        Capteur[0] = s1.ValueDistance(DataReceive) * 0xFFF / 300;
        Capteur[1] = s2.ValueDistance(DataReceive) * 0xFFF / 300;
        Capteur[2] = s3.ValueDistance(DataReceive) * 0xFFF / 300;
        SM[0] = MotorA.SpeedMotor();
        SM[1] = MotorB.SpeedMotor();
        SM[2] = MotorC.SpeedMotor();
        SM[3] = MotorD.SpeedMotor();
        if (Data != OldData)
        {
            OldData = Data;
            MotorA.Hc595WriteByte(OldData);
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
