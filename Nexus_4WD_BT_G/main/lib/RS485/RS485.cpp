#include "RS485.h"

extern "C" {
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
#define ECHO_TEST_TXD (23)
#define ECHO_TEST_RXD (22)
#define ECHO_TEST_RTS (21)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)
#define BUF_SIZE (127)
#define BAUD_RATE (19200)
#define PACKET_READ_TICS (200 / portTICK_PERIOD_MS)
//#define ECHO_UART_PORT 2
#define ECHO_READ_TOUT (3)
#define TAG "RS485_ECHO_APP"


#pragma region RS485
void InitRS485()
{
    uart_port_t uart_num = (uart_port_t) (2);
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_DEFAULT,
    };
    esp_log_level_set(TAG, ESP_LOG_INFO);
    ESP_ERROR_CHECK(uart_driver_install(uart_num, BUF_SIZE*2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));
    ESP_ERROR_CHECK(uart_set_mode(uart_num, UART_MODE_RS485_HALF_DUPLEX));
    ESP_ERROR_CHECK(uart_set_rx_timeout(uart_num, ECHO_READ_TOUT));
}

static void echo_send(uart_port_t port, const char* str, int length)
{
    if (uart_write_bytes(port, str, length) != length) {
        abort();
    }
}
void RS485Send(uart_port_t uart_num, const char* data, int length)
{
    echo_send(uart_num, data, length);
}

int RS485Receive(uart_port_t uart_num)
{
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);

    int len = uart_read_bytes(uart_num, data, BUF_SIZE, PACKET_READ_TICS);
    if (len > 0)
    {
        for (int i = 0; i < len; i++)
        {
            DataReceive[i] = data[i];
        }
        return len;
    }
    else
    {
        return 0;
    }
}

#pragma endregion

#pragma region Sensor

void Sensor::SensorAdress(uint8_t Address)
{
    _Address = Address;
    sendSetTrigger[2] = _Address;
    sendDistance[2] = _Address;
    sendTemp[2] = _Address;
    sendSetTrigger[5] = (_Address + 0x100 )%0x100; // 0x100 = sum without adress
    sendDistance[5] = (_Address + 0x101)%0x100; // 0x101 = sum without adress
    sendTemp[5] = (_Address + 0x102)%0x100; // 0x102 = sum without adress
}

void Sensor::InitSensorTrigger()
{
    
    RS485Send((uart_port_t)2, sendSetTrigger, 6);
}
int Sensor::GetDistance()
{
    RS485Send((uart_port_t)2, sendDistance, (int)6);
    int len = RS485Receive((uart_port_t)2);
    return len;
}

int Sensor::GetTemp()
{
    RS485Send((uart_port_t) 2, sendTemp, (int)6);
    int len = RS485Receive((uart_port_t) 2);
    return len;
}

int Sensor::ValueDistance(char data[126])
{
    int distance = 0;
    int sum = 0;
    if (data[0] == 0x55 && data[1] == 0xaa && data[2] == _Address && data[3] == 0x02 && data[4] == 0x02)
    {
        distance = (data[5] << 8) + data[6]; // read distance
        for (int i = 0; i < 7; i++)
        {
            sum = sum + data[i];
        }
        sum = sum % 0x100;
        if (data[7] == sum)
            return distance;
    }
    return -1;
}

float Sensor::ValueTemp(char data[126])
{
    float temp = 0;
    int sum = 0;
    if (data[0] == 0x55 && data[1] == 0xaa && data[2] == _Address && data[3] == 0x02 && data[4] == 0x03)
    {
        if (data[5]>>4 == 0x0)
        {
            temp = (((data[5]<<8)&0x0FFF) + data[6])*0.1; // *0.1 beacause the temps is given by the sensor to temp*10
        }
        else
        {
            temp = -((((data[5]<<8)&0x0FFF) + data[6]))*0.1; // temps negatif
        }
        for (int i = 0; i < 7; i++)
        {
            sum = sum + data[i];
        }
        sum = sum % 0x100;
        if (data[7] == sum)
            return temp;
    }
    return -1;
}
#pragma endregion
