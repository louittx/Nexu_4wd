
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
// pin for the motor
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

// Pin fot the 74hc595
#define DS GPIO_NUM_16
#define STCP GPIO_NUM_4
#define SHCP GPIO_NUM_0

#pragma endregion
#pragma region Define Variables
#define FrequencyMotor 300000
#define DutyResolutionMotor LEDC_TIMER_8_BIT // LEDC_TIMER_8_BIT = 0 to 255
#pragma endregion

#pragma endregion

#pragma region Variables

// variabel for the blutooth
int SpeedMessage;
int AngleMessage;
int DirectionMessage;
int Capteur[3];
int SM[4];
bool SendMessage = true;
u_int8_t get_message[16];
char Buffer[16];
u_int8_t AddressMotor = 0x44;
u_int8_t AddressSensor = 0x45;
uint8_t LengMessage = 0;
uint8_t LengGetMessage = 0;


// variable for the RS485
char DataReceive[126];

// variable for the 74HC595
int Data = 0;
int OldData = 0;
int length = 0;

#pragma endregion


// motif varible for ce PID
/*SHELL_PARAMETER_FLOAT(kp, "kp", 1);
SHELL_PARAMETER_FLOAT(ki, "ki", 2.0);
SHELL_PARAMETER_FLOAT(kd, "kd", 0.001);*/

//variable for the PID not motif
const uint8_t kp = 1;
const uint8_t ki = 2;
const float kd = 0.001;

// defined the motor
MotorEncoderHc595 MotorA;
MotorEncoderHc595 MotorB;
MotorEncoderHc595 MotorC;
MotorEncoderHc595 MotorD;

// defined the Sensor 
Sensor s1;
Sensor s2;
Sensor s3;

#pragma region functions

// fonctiont for the convert 3 msg Sensor in 1 msg Sensor
u_int64_t MesssageToSensor(int16_t Sensor1, int16_t Sensor2, int16_t Sensor3)
{
    u_int64_t Message = 0;
    Message |= ((u_int64_t)(Sensor1 & 0xFFF) << 24);
    Message |= ((u_int64_t)(Sensor2 & 0xFFF) << 12);
    Message |= (u_int64_t)(Sensor3 & 0xFFF);
    return Message;
}

// fonctiont for the send the msg in the buffer
void SendMessageToSensor(u_int64_t Message)
{
    for (int i = 0; i < 5; i++)
    {
        Buffer[i + 1] = (Message >> ((8 * 4) - (8 * i))) & 0xFF;
    }
}

#pragma endregion


#pragma region Functions Task
// task for say the Message then send Message
void ValueSendMessage(void *pvParameters)
{
    while (1)
    {
        bool EtatSendMessage = SendMessage; // inverce sendMessage for switch for the send message 
        
        // send the motor message
        if (EtatSendMessage == true)
        {
            LengMessage = 5; // length of message to motor
#pragma region MsgMotor
            Buffer[0] = AddressMotor; // set adresse to the buffer
            for (int i = 0; i < 4; i++)
            {
                Buffer[i + 1] = SM[i] * 255 / 7300; // Tranforme the SM 0 to 7300 into 0 to 255
            }
#pragma endregion
        }
        // send the Sensor Message
        if (EtatSendMessage == false)
        {
            LengMessage = 6; // length message
#pragma region MsgSensor
            Buffer[0] = AddressSensor; // set adresse
            u_int64_t MessageSensor = MesssageToSensor(Capteur[0], Capteur[1], Capteur[2]); // convret 3 sensor to the long
            SendMessageToSensor(MessageSensor); // send message 

#pragma endregion
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
// task for tranforme the get msg to variable
void ValueGetMessage(void *pvParameters)
{
    while (1)
    {
        {
            if ((get_message[0] == 0x17) && (LengGetMessage == 4))
            {
                SpeedMessage = get_message[1]; // recup the value of the message
                AngleMessage = get_message[2];
                DirectionMessage = get_message[3];
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
//task for the send and receviede the dist thanks to RS485
static void RS485(void *arg)
{
    InitRS485();
    while (1)
    {
        for (int i = 0; i < 3; i++) // repaiter for the 3 capteur
        {
            switch (i)
            {
            case 0:
                s1.InitSensorTrigger(); // send onde for the trigger
                vTaskDelay(80 / portTICK_PERIOD_MS); // delay for the capteur is the time of receveite value
                length = s1.GetDistance(); // allow stok the dist
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
            vTaskDelay(30/ portTICK_PERIOD_MS);
        }
        // code for the reseveid the temp
        /* length = s1.GetTemp(); */
    }
}
// task for the control the speed Motor
void SetSpeeds(void *arg)
{
    while (1)
    {
        float Consigne = SpeedMessage * 7500.0 / 255.0; // convert speedMessage 0 to 255 into 0 to 7500.0
        MotorA.SetSpeedPID(Consigne, SM[0], kp, ki, kd); // set speed Motor
        MotorB.SetSpeedPID(Consigne, SM[1], kp, ki, kd);
        MotorC.SetSpeedPID(Consigne, SM[2], kp, ki, kd);
        MotorD.SetSpeedPID(Consigne, SM[3], kp, ki, kd);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
// task for the control the dirction of robot
void SetAngle(void *arg)
{
    while (1)
    {
        uint16_t GetAngle = (AngleMessage * 360) / 0xFF; // convet the msg to angle ranging from 0 to 360
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
    //for the modifer the variable PID
    /*shell_init(115200);
    shell_start_task();*/
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
    
    // start the task
    xTaskCreate(SetAngle, "SetAngle", 2048, NULL, 0, NULL);
    xTaskCreate(SetSpeeds, "SetSpeed", 2048, NULL, 4, NULL);
    xTaskCreate(RS485, "RS485", 2048, NULL, 2, NULL);
    xTaskCreate(ValueGetMessage, "GetMsg", 4096, NULL, 5, NULL);
    xTaskCreate(ValueSendMessage, "SendMsg", 4096, NULL, 3, NULL);


    while (1)
    {
        Capteur[0] = s1.ValueDistance(DataReceive) * 0xFFF / 300; // convert the diatnce of 0 to 300 in 0 to FFF
        Capteur[1] = s2.ValueDistance(DataReceive) * 0xFFF / 300;
        Capteur[2] = s3.ValueDistance(DataReceive) * 0xFFF / 300;
        SM[0] = MotorA.SpeedMotor(); // read speed of motor
        SM[1] = MotorB.SpeedMotor();
        SM[2] = MotorC.SpeedMotor();
        SM[3] = MotorD.SpeedMotor();
        
        if (Data != OldData) // for not change if there is nothing to change
        {
            OldData = Data;
            MotorA.Hc595WriteByte(OldData); // push th confiration to the pins in the 74HC595
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
