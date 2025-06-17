
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
#include "esp32/rom/ets_sys.h"
#include <math.h>
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
char Buffer[1023];
u_int8_t AddressMotor = 0x44;
u_int8_t AddressSensor = 0x45;
uint8_t LengMessage = 0;
uint8_t LengGetMessage = 0;
char bufferSensor[16];
char bufferEncoder[16];

// variable for the RS485
char DataReceive[126];

// variable for the 74HC595
int Data = 0;
int OldData = 0;
int length = 0;

// motif varible for ce PID
/*SHELL_PARAMETER_FLOAT(kp, "kp", 1);
SHELL_PARAMETER_FLOAT(ki, "ki", 2.0);
SHELL_PARAMETER_FLOAT(kd, "kd", 0.001);*/

// variable for the PID not motif
const uint8_t kp = 1;
const uint8_t ki = 2;
const float kd = 0.001;

// variable for the motor
int PuissanceSpeeds;

int ConsigneA;
int NewConsigneA;
int OldConsigneA;
uint8_t SensA = 1;

int ConsigneB;
int NewConsigneB;
int OldConsigneB;
uint8_t SensB = 1;

int ConsigneC;
int NewConsigneC;
int OldConsigneC;
uint8_t SensC = 1;

int ConsigneD;
int NewConsigneD;
int OldConsigneD;
uint8_t SensD = 1;

#pragma endregion

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
    Message |= (u_int64_t)((Sensor1 & 0xFFF) << 24);
    Message |= (u_int64_t)((Sensor2 & 0xFFF) << 12);
    Message |= (u_int64_t)(Sensor3 & 0xFFF);
    return Message;
}

// fonctiont for the send the msg in the buffer
void SendMessageToSensor(u_int64_t Message)
{
    for (int i = 0; i < 5; i++)
    {
        bufferSensor[i + 1] = (Message >> ((8 * 4) - (8 * i))) & 0xFF;
    }
}

//fonctiont for the set speeds motor and direction
void Direction(int Speed, float Dirc, float orian)
{

    int SpeedsAD = Speed * (sin(Dirc) - cos(Dirc)); // set speeds for the moves without rotation
    int SpeedsBC = Speed * (sin(Dirc) + cos(Dirc));
    if (abs(orian) < 0.1)
    {
        ConsigneA = (SpeedsAD); // set speeds with rotation
        ConsigneB = (SpeedsBC);
        ConsigneC = (SpeedsBC);
        ConsigneD = (SpeedsAD);
    }
    else
    {
        ConsigneA = (SpeedsAD)*orian; // set speeds with rotation
        ConsigneB = (SpeedsBC)*orian;
        ConsigneC = (SpeedsBC) * -orian;
        ConsigneD = (SpeedsAD) * -orian;
    }
    ESP_LOGI("Dirc","%d", Dirc);
    //ESP_LOGI("orian2","%d", DirectionMessage);
    MotorA.SetSpeedPID(abs(NewConsigneA), abs(SM[0]), kp, ki, kd); // activates the motor
    MotorB.SetSpeedPID(abs(NewConsigneB), abs(SM[1]), kp, ki, kd);
    MotorC.SetSpeedPID(abs(NewConsigneC), abs(SM[2]), kp, ki, kd);
    MotorD.SetSpeedPID(abs(NewConsigneD), abs(SM[3]), kp, ki, kd);
}

#pragma endregion

#pragma region Functions Task
// task for say the Message then send Message

void SendMessageBT(void *pvParameters)
{
    while (1)
    {
        bool EtatSendMessage = SendMessage;
        if (EtatSendMessage)
        {
            LengMessage = 5;
            for (int i = 0; i < LengMessage; i++)
            {
                Buffer[i] = bufferEncoder[i];
            }
        }
        else
        {
            LengMessage = 6;
            for (int i = 0; i < LengMessage; i++)
            {
                Buffer[i] = bufferSensor[i];
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void ValueSendMessage(void *pvParameters)
{
    while (1)
    {
        // the motor message
#pragma region MsgMotor
        bufferEncoder[0] = AddressMotor; // set adresse to the buffer
        for (int i = 0; i < 4; i++)
        {
            bufferEncoder[i + 1] = SM[i] * 255 / 7300; // Tranforme the SM 0 to 7300 into 0 to 255
        }
#pragma endregion
        // the Sensor Message
#pragma region MsgSensor
        bufferSensor[0] = AddressSensor;                                                // set adresse
        u_int64_t MessageSensor = MesssageToSensor(Capteur[0], Capteur[1], Capteur[2]); // convret 3 sensor to the long
        SendMessageToSensor(MessageSensor);                                             // send message

#pragma endregion
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
// task for tranforme the get msg to variable
void ValueGetMessage(void *pvParameters)
{
    while (1)
    {
        if ((get_message[0] == 0x17) && (LengGetMessage == 4))
        {
            SpeedMessage = get_message[1]; // recup the value of the message
            AngleMessage = get_message[2];
            DirectionMessage = get_message[3];
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
// task for the send and receviede the dist thanks to RS485
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
                s1.InitSensorTrigger();
                vTaskDelay(pdMS_TO_TICKS(80));                            // delay for the capteur is the time of receveite value
                length = s1.GetDistance();                                // allow stok the dist
                Capteur[0] = s1.ValueDistance(DataReceive) * 0xFFF / 300; // convert the diatnce of 0 to 300 in 0 to FFF
                break;
            case 1:
                s2.InitSensorTrigger();
                vTaskDelay(pdMS_TO_TICKS(80));
                length = s2.GetDistance();
                Capteur[1] = s2.ValueDistance(DataReceive) * 0xFFF / 300;
                break;
            case 2:
                s3.InitSensorTrigger();
                vTaskDelay(pdMS_TO_TICKS(80));
                length = s3.GetDistance();
                Capteur[2] = s3.ValueDistance(DataReceive) * 0xFFF / 300;
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(30));
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
        float Angle = ((AngleMessage * 360) / 0xFF); // convert angle of 0 to 255 in 0 to 360
        int Quartier = ((int)((Angle + 22.5) / 45)) % 8; // offset to 22.5°
        float GetAngle =  -((Quartier * (2 * M_PI) )/ 8); // convert in radian, the negatif beauce the Angle is turns clockwise, and we can turns trigonometric sense 
        if (sin(GetAngle) < 0)
        {
            PuissanceSpeeds = -(SpeedMessage * 5300.0 / 255.0); // Speeds negative and convert
        }
        else
        {
            PuissanceSpeeds = SpeedMessage * 5300.0 / 255.0;
        }
        float oriantation = (((DirectionMessage *2.0)/ 255.0) - 1.0); // convert oriantation of 0 to 255 in -1 to 1
        //ESP_LOGI("GetAngle = ", "%f", (GetAngle));
        //ESP_LOGI("GetAngle + = ", "%f", (sin(GetAngle)+cos(GetAngle)));
        //ESP_LOGI("GetAngle - = ", "%f", (sin(GetAngle)-cos(GetAngle)));
        //ESP_LOGI("oriantation = ", "%f", oriantation);
        Direction(PuissanceSpeeds, GetAngle, oriantation); // start the fonction for control motor
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
// task for the control the Invrec Direction
void SetConsigneA(void *arg)
{
    while (1)
    {
        NewConsigneA = ConsigneA;
        int i = 0;
        while (SM[0] != NewConsigneA && (i != 50))
        {
            if (OldConsigneA * NewConsigneA < 0)
            {
                if (NewConsigneA < 0)
                {
                    SensA = 2;
                }
                else if (NewConsigneA > 0)
                {
                    SensA = 1;
                }
                NewConsigneA = 0;
            }
            i++;
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        Data = MotorA.DirHc595(SensA);
        OldConsigneA = NewConsigneA;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
// tasck for teh set teh conisgne for the PID 
void SetConsigneB(void *arg)
{
    while (1)
    {
        NewConsigneB = ConsigneB;
        int i = 0;
        while (SM[1] != NewConsigneB && (i != 50))
        {
            if (OldConsigneB * NewConsigneB < 0)
            {
                if (NewConsigneB < 0)
                {
                    SensB = 2;
                }
                else if (NewConsigneB > 0)
                {
                    SensB = 1;
                }
                NewConsigneB = 0;
            }
            i++;
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        Data = MotorB.DirHc595(SensB);
        OldConsigneB = NewConsigneB;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void SetConsigneC(void *arg)
{
    while (1)
    {
        NewConsigneC = ConsigneC;
        int i = 0;
        while (SM[2] != NewConsigneC && (i != 50))
        {
            if (OldConsigneC * NewConsigneC < 0)
            {
                if (NewConsigneC < 0)
                {
                    SensC = 2;
                }
                else if (NewConsigneC > 0)
                {
                    SensC = 1;
                }
                NewConsigneC = 0;
            }
            i++;
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        Data = MotorC.DirHc595(SensC);
        OldConsigneC = NewConsigneC;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void SetConsigneD(void *arg)
{
    while (1)
    {
        NewConsigneD = ConsigneD;
        int i = 0;
        while (SM[3] != NewConsigneD && (i != 50))
        {
            if (OldConsigneD * NewConsigneD < 0)
            {
                if (NewConsigneD < 0)
                {
                    SensD = 2;
                }
                else if (NewConsigneD > 0)
                {
                    SensD = 1;
                }
                NewConsigneD = 0;
            }
            i++;
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        Data = MotorD.DirHc595(SensD);
        OldConsigneD = NewConsigneD;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
// task to get the speeds of the motors
void ValueSpeedsMotor(void *arg)
{
    while (1)
    {
    SM[0] = MotorA.SpeedMotor(); // read speed of motor
    SM[1] = MotorB.SpeedMotor();
    SM[2] = MotorC.SpeedMotor();
    SM[3] = MotorD.SpeedMotor();
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
    // for the modifer the variable PID
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
    xTaskCreate(SetConsigneA, "SetConsigneA", 1024, NULL, 0, NULL);
    xTaskCreate(SetConsigneB, "SetConsigneB", 1024, NULL, 1, NULL);
    xTaskCreate(SetConsigneC, "SetConsigneC", 1024, NULL, 2, NULL);
    xTaskCreate(SetConsigneD, "SetConsigneD", 1024, NULL, 3, NULL);
    xTaskCreate(ValueSpeedsMotor, "ValueSpeedsMotor",2048,NULL,4,NULL);
    xTaskCreate(RS485, "RS485", 1024, NULL, 5, NULL);
    xTaskCreate(SetSpeeds, "SetSpeed", 4096, NULL, 6, NULL);
    xTaskCreate(SendMessageBT, "sendMsg", 4096, NULL, 7, NULL);
    xTaskCreate(ValueSendMessage, "ValueMsg", 4096, NULL, 8, NULL);
    xTaskCreate(ValueGetMessage, "GetMsg", 4096, NULL, 9, NULL);

    while (1)
    {
        if (Data != OldData) // for not change if there is nothing to change
        {
            OldData = Data;
            MotorA.Hc595WriteByte(OldData); // push th confiration to the pins in the 74HC595
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
