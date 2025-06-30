#ifndef config_h
#define config_h

extern int Data;

extern "C"
{
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
#include "esp32/rom/ets_sys.h"
}

void EncoderInit( gpio_num_t PinA,  gpio_num_t PinB, pcnt_unit_t PcntUnit,  uint16_t Limit);
int16_t EncoderSetCount(pcnt_unit_t PcntUnit);
int EncoderClear(pcnt_unit_t PcntUnit);
void EncoderPause(pcnt_unit_t PcntUnit);

void PWMInit(gpio_num_t Pin, uint32_t Frequency, ledc_timer_bit_t DutyResolution, ledc_channel_t channelle);
void PWMlevel(ledc_channel_t channel, int duty);

void ConfigGPIO(gpio_num_t Pin);
void GPIOSetLevel(gpio_num_t Pin, uint8_t Level);

class Motor
{
public:
    void MotorAttached(gpio_num_t PWM, gpio_num_t PinA, gpio_num_t PinB, ledc_channel_t channel);
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
    float _integrl;
    float _OldError;
};

class MotorEncoder
{
public:
    void MotorAttached(gpio_num_t PWM, gpio_num_t IntA, gpio_num_t IntB, ledc_channel_t channel);
    void MotorResolution(uint32_t Frequency, ledc_timer_bit_t DutyResolution);
    void EncodeurAttached(gpio_num_t PinA, gpio_num_t PinB, pcnt_unit_t PcntUnit, uint64_t limit);
    void InitMotorEncodeur();
    void SetSpeed(int Speed);
    void SetDirection(int Direction);
    float SpeedMotor();
    void SetSpeedPID(int consigne, float Speeds, float Kp, float Ki, float Kd);

private:
    gpio_num_t _PWM;
    gpio_num_t _IntA;
    gpio_num_t _IntB;
    ledc_channel_t __channel;
    uint32_t _Frequency;
    ledc_timer_bit_t _DutyResolution;
    gpio_num_t _PinA;
    gpio_num_t _PinB;
    pcnt_unit_t _PcntUnit;
    uint64_t _limit;
    float Integral;
    float OldError;
};

class MotorEncoderHc595
{
    public:
        void MotorAttached(gpio_num_t PWM, ledc_channel_t channel, uint8_t motor);
        void MotorResolution(uint32_t Frequency, ledc_timer_bit_t DutyResolution);
        void EncodeurAttached(gpio_num_t PinA, gpio_num_t PinB, pcnt_unit_t PcntUnit, uint64_t limit);
        void hc595Attached(gpio_num_t dataPin, gpio_num_t clockPin, gpio_num_t latchPin);
        void InitMotorEncodeurHC595();
        void SetSpeed(int Speed);
        float SpeedMotor();
        void SetSpeedPID(int consigne, float Speeds, float Kp, float Ki, float Kd);
        int DirHc595(int dir);
        void Hc595WriteByte(uint8_t data);
    private:
        uint8_t _motor;
        gpio_num_t _PWM;
        ledc_channel_t __channel;
        uint32_t _Frequency;
        ledc_timer_bit_t _DutyResolution;
        gpio_num_t _PinA;
        gpio_num_t _PinB;
        pcnt_unit_t _PcntUnit;
        uint64_t _limit;
        float Integral;
        float OldError;
        gpio_num_t _dataPin;
        gpio_num_t _clockPin;
        gpio_num_t _latchPin;
};
#endif
