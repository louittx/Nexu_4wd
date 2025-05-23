#include "config.h"

#pragma region Encoder

void EncoderInit(gpio_num_t PinA, gpio_num_t PinB, pcnt_unit_t PcntUnit, uint16_t Limit)
{
    pcnt_config_t pcnt_config{};
    pcnt_config.pulse_gpio_num = PinA;
    pcnt_config.ctrl_gpio_num = PinB;
    pcnt_config.channel = PCNT_CHANNEL_0;
    pcnt_config.unit = PcntUnit;
    pcnt_config.pos_mode = PCNT_COUNT_INC;
    pcnt_config.neg_mode = PCNT_COUNT_DEC;
    pcnt_config.lctrl_mode = PCNT_MODE_REVERSE;
    pcnt_config.hctrl_mode = PCNT_MODE_KEEP;
    pcnt_config.counter_h_lim = Limit;
    pcnt_config.counter_l_lim = -Limit;

    pcnt_unit_config(&pcnt_config);
    pcnt_counter_pause(PcntUnit);
    pcnt_counter_clear(PcntUnit);
    pcnt_counter_resume(PcntUnit);
}

int16_t EncoderSetCount(pcnt_unit_t PcntUnit)
{
    int16_t count = 0;
    pcnt_get_counter_value(PcntUnit, &count);
    return count;
}
int EncoderClear(pcnt_unit_t PcntUnit)
{
    pcnt_counter_clear(PcntUnit);
    return 0;
}
void EncoderPause(pcnt_unit_t PcntUnit)
{
    pcnt_counter_pause(PCNT_UNIT_0);
}

#pragma endregion

#pragma region PWM

void PWMInit(gpio_num_t Pin, uint32_t Frequency, ledc_timer_bit_t DutyResolution, ledc_channel_t channelle)
{
    ledc_timer_config_t ledc_timer = {};
    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_timer.timer_num = LEDC_TIMER_0;
    ledc_timer.duty_resolution = DutyResolution;
    ledc_timer.freq_hz = Frequency;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel{};
    ledc_channel.channel = channelle;
    ledc_channel.duty = 0;
    ledc_channel.gpio_num = Pin;
    ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel.timer_sel = LEDC_TIMER_0;
    ledc_channel.hpoint = 0;
    ledc_channel.intr_type = LEDC_INTR_DISABLE;

    auto ret = ledc_channel_config(&ledc_channel);
    if (ret != ESP_OK)
    {
        ESP_LOGE("PWM", "ledc_channel_config failed: %s", esp_err_to_name(ret));
        ESP_LOGI("PWM", "ledc_channel_config:");
    }

    PWMlevel(channelle, 0);
}
void PWMlevel(ledc_channel_t channel, int duty)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
}

#pragma endregion

#pragma region GPIO

void ConfigGPIO(gpio_num_t Pin)
{
    gpio_config_t io_conf_led = {
        .pin_bit_mask = (1ULL << Pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_conf_led);
    GPIOSetLevel(Pin, 0);
}
void GPIOSetLevel(gpio_num_t Pin, uint8_t Level)
{
    gpio_set_level(Pin, Level);
}

#pragma endregion

#pragma region Motor

void Motor::MotorAttached(gpio_num_t PWM, gpio_num_t PinA, gpio_num_t PinB, ledc_channel_t channel)
{
    PinPWM = PWM;
    In_1 = PinA;
    In_2 = PinB;
    _channel = channel;
}
void Motor::MotorResolution(uint32_t Frequency, ledc_timer_bit_t DutyResolution)
{
    _Frequency = Frequency;
    _DutyResolution = DutyResolution;
}
void Motor::InitMotor()
{
    PWMInit(PinPWM, _Frequency, _DutyResolution, _channel);
    ConfigGPIO(In_1);
    ConfigGPIO(In_2);
    Motor::SetSpeed(0);
    Motor::SetDirection(0);
}

void Motor::SetSpeed(int Speed)
{
    if (Speed > 254)
        Speed = 254;
    else if (Speed < 1)
        Speed = 0;
    PWMlevel(_channel, Speed);
}
void Motor::SetDirection(int Direction)
{
    switch (Direction)
    {
    case 0:
        GPIOSetLevel(In_1, 0);
        GPIOSetLevel(In_2, 0);
        break;
    case 1:
        GPIOSetLevel(In_1, 1);
        GPIOSetLevel(In_2, 0);
        break;
    case 2:
        GPIOSetLevel(In_1, 0);
        GPIOSetLevel(In_2, 1);
        break;
    default:
        break;
    }
}

#pragma endregion

#pragma region EncodeurMotor

void MotorEncoder::MotorAttached(gpio_num_t PWM, gpio_num_t IntA, gpio_num_t IntB, ledc_channel_t channel)
{
    _PWM = PWM;
    _IntA = IntA;
    _IntB = IntB;
    __channel = channel;
}
void MotorEncoder::MotorResolution(uint32_t Frequency, ledc_timer_bit_t DutyResolution)
{
    _Frequency = Frequency;
    _DutyResolution = DutyResolution;
}
void MotorEncoder::EncodeurAttached(gpio_num_t PinA, gpio_num_t PinB, pcnt_unit_t PcntUnit, uint64_t limit)
{
    _PinA = PinA;
    _PinB = PinB;
    _PcntUnit = PcntUnit;
    _limit = limit;
}
void MotorEncoder::InitMotorEncodeur()
{
    PWMInit(_PWM, _Frequency, _DutyResolution, __channel);
    ConfigGPIO(_IntA);
    ConfigGPIO(_IntB);
    EncoderInit(_PinA, _PinB, _PcntUnit, _limit);
    Integral = 0;
    OldError = 0;
    MotorEncoder::SetSpeed(0);
    MotorEncoder::SetDirection(0);
}

void MotorEncoder::SetSpeed(int Speed)
{
    if (Speed > 254)
        Speed = 254;
    else if (Speed < 1)
        Speed = 0;
    PWMlevel(__channel, Speed);
}
void MotorEncoder::SetDirection(int Direction)
{
    switch (Direction)
    {
    case 0:
        GPIOSetLevel(_IntA, 0);
        GPIOSetLevel(_IntB, 0);
        break;
    case 1:
        GPIOSetLevel(_IntA, 1);
        GPIOSetLevel(_IntB, 0);
        break;
    case 2:
        GPIOSetLevel(_IntA, 0);
        GPIOSetLevel(_IntB, 1);
        break;
    default:
        break;
    }
}

float MotorEncoder::SpeedMotor()
{
    int count = EncoderClear(_PcntUnit);
    count = EncoderSetCount(_PcntUnit);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    count = EncoderSetCount(_PcntUnit);
    float vitesse_moteur = (((count) * (60 / 0.01)) / 24);
    vitesse_moteur = vitesse_moteur;
    return vitesse_moteur;
}

void MotorEncoder::SetSpeedPID(int consigne, float Speeds, float Kp, float Ki, float Kd)
{
    if (Speeds == -1)
    {
        Speeds = MotorEncoder::SpeedMotor();
    }
    float SpeedNorm = Speeds / 7650.0;
    float ConsigneNorm = consigne / 7650.0;
    float Error = ConsigneNorm - SpeedNorm;
    Integral += Error * 0.01;
    float Derivative = (Error - OldError) / 0.01;
    float Sortie = Kp * Error + Ki * Integral + Kd * Derivative;
    float PWM = Sortie * 254.0;
    if (PWM > 254)
        PWM = 254;
    else if (PWM < 0)
        PWM = 0;
    OldError = Error;
    MotorEncoder::SetSpeed(PWM);
}

#pragma endregion