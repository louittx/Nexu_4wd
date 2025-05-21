
#pragma region Includes
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#pragma endregion
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
#include "rhoban_shell.h"
#include "lib/config/config.h"


#pragma region Defines

#pragma region Define Pin
#define PinPWM2 GPIO_NUM_21

#define PinPWMA GPIO_NUM_19
#define PinInA_1 GPIO_NUM_18
#define PinInA_2 GPIO_NUM_5

#define PinPWMB GPIO_NUM_17
#define PinInB_1 GPIO_NUM_16
#define PinInB_2 GPIO_NUM_4

#define PinPWMC GPIO_NUM_0
#define PinInC_1 GPIO_NUM_2
#define PinInC_2 GPIO_NUM_15

#define PinPWMD GPIO_NUM_27
#define PinInD_1 GPIO_NUM_14
#define PinInD_2 GPIO_NUM_12

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
u_int8_t AddressMotor = 0x44 ;
u_int8_t AddressSensor = 0x45;
char MotorandSensorBuffer[32];

u_int32_t MotorAndSensorMessage = 0;
uint32_t ValueMotorAndSensor = 0;
uint8_t LengMessage = 0;
uint8_t LengGetMessage = 0;

#pragma endregion


SHELL_PARAMETER_INT(Speed1, "Speed1", 0);
SHELL_PARAMETER_INT(Speed2, "Speed2", 0);
SHELL_PARAMETER_INT(Speed3, "Speed3", 0);
SHELL_PARAMETER_INT(Speed4, "Speed4", 0);
SHELL_PARAMETER_INT(Capteur1, "Capteur1", 0);
SHELL_PARAMETER_INT(Capteur2, "Capteur2", 0);
SHELL_PARAMETER_INT(Capteur3, "Capteur3", 0);

Motor motorA;
Motor motorB;
Motor motorC;
Motor motorD;

#pragma region functions
u_int64_t MesssageToSensor(int16_t Sensor1, int16_t Sensor2, int16_t Sensor3)
{
    u_int64_t Message = 0;
    Message = (Sensor1 & 0xFFF) << 24;
    Message = Message + ((Sensor2 & 0xFFF) << 12);
    Message = Message + (Sensor3 & 0xFFF);
    Message = Message & 0xFFFFFFFFF;
    return Message;
}

void SendMessageToSensor(u_int64_t Message){
    for (int i = 0; i<5; i++){
        Buffer[i+1] = (Message >>((8*4)-(8*i))) & 0xFF;
    }
}

#pragma endregion

#pragma region Functions Task
void MotorSpeed(void *pvParameters)
{
    while (1)
    {
        motorA.SetSpeed(SpeedMessage);
        motorB.SetSpeed(SpeedMessage);
        motorC.SetSpeed(SpeedMessage);
        motorD.SetSpeed(SpeedMessage);
        //printf("SpeedMessage : %d\n", SpeedMessage);
        DirectionMotor = ((DirectionMessage * 2) + 1) / 0xFF;
        motorA.SetDirection(DirectionMotor);
        motorB.SetDirection(DirectionMotor);
        motorC.SetDirection(DirectionMotor);
        motorD.SetDirection(DirectionMotor);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

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
#pragma region InitMotor
    motorA.MotorAttached(PinPWMA, PinInA_1, PinInA_2, LEDC_CHANNEL_0);
    motorA.MotorResolution(FrequencyMotor, DutyResolutionMotor);
    motorA.InitMotor();
    motorB.MotorAttached(PinPWMB, PinInB_1, PinInB_2, LEDC_CHANNEL_1);
    motorB.MotorResolution(FrequencyMotor, DutyResolutionMotor);
    motorB.InitMotor();
    motorC.MotorAttached(PinPWMC, PinInC_1, PinInC_2, LEDC_CHANNEL_2);
    motorC.MotorResolution(FrequencyMotor, DutyResolutionMotor);
    motorC.InitMotor();
    motorD.MotorAttached(PinPWMD, PinInD_1, PinInD_2, LEDC_CHANNEL_3);
    motorD.MotorResolution(FrequencyMotor, DutyResolutionMotor);
    motorD.InitMotor();
#pragma endregion
    xTaskCreate(MotorSpeed, "SetSpeed", 2048, NULL, 0, NULL);
    xTaskCreate(ValueSendMessage, "SendMsg", 4096, NULL, 3, NULL);
    xTaskCreate(ValueGetMessage, "GetMsg", 4096, NULL, 2, NULL);
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
        Capteur[0] = Capteur1;
        Capteur[1] = Capteur2;
        Capteur[2] = Capteur3;
        VM[0] = Speed1;
        VM[1] = Speed2;
        VM[2] = Speed3;
        VM[3] = Speed4;
    }
}


