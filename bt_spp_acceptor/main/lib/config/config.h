#ifndef config_h
#define config_h

extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "driver/pcnt.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "soc/gpio_sig_map.h"
}

void EncoderInit( gpio_num_t PinA,  gpio_num_t PinB,  uint16_t Limit);
int16_t EncoderSetCount();
void EncoderClear();
void EncoderPause();

void PWMInit(gpio_num_t Pin,  uint32_t Frequency, ledc_timer_bit_t DutyResolution, ledc_channel_t channelle);
void PWMlevel(ledc_channel_t channel, int duty);

void ConfigGPIO( gpio_num_t Pin);
void GPIOSetLevel( gpio_num_t Pin,  uint8_t Level);

class Motor
{
public:
    void MotorAttached( gpio_num_t PWM, gpio_num_t PinA,  gpio_num_t PinB, ledc_channel_t channel);
    void MotorResolution(uint32_t Frequency, ledc_timer_bit_t DutyResolution);
    void InitMotor();
    void SetSpeed(int Speed);
    void SetDirection(int Direction);
private:
    gpio_num_t PinPWM;
    gpio_num_t In_1;
    gpio_num_t In_2;
    ledc_channel_t _channel;
    uint32_t _Frequency;
    ledc_timer_bit_t _DutyResolution;
};

#endif
