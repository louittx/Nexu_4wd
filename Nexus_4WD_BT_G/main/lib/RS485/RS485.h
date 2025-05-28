#ifndef RS485_h
#define RS485_h

extern "C"
{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "sdkconfig.h"
}

extern char DataReceive[126];

void InitRS485();
static void echo_send(uart_port_t port, const char* str, int length);
void RS485Send(uart_port_t uart_num, char data, int length);
int RS485Receive(uart_port_t uart_num);

class Sensor
{
public:
    void SensorAdress(uint8_t Address);
    void InitSensorTrigger();
    int GetDistance();
    int GetTemp();
    int ValueDistance(char data[126]);
    float ValueTemp(char data[126]);

private:
    uint8_t _Address;
    char sendSetTrigger[6] = {0x55, 0xaa, 0x11, 0x00, 0x01, 0x11};
    char sendDistance[6] = {0x55, 0xaa, 0x11, 0x00, 0x02, 0x12};
    char sendTemp[6] = {0x55, 0xaa, 0x11, 0x00, 0x03, 0x13};
};



#endif