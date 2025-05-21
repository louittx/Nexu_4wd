#include "config.h"

#pragma region Encoder

void EncoderInit( gpio_num_t PinA,  gpio_num_t PinB,  uint16_t Limit)
{
    pcnt_config_t pcnt_config{};
    pcnt_config.pulse_gpio_num = PinA;
    pcnt_config.ctrl_gpio_num = PinB;
    pcnt_config.channel = PCNT_CHANNEL_0;
    pcnt_config.unit = PCNT_UNIT_0;
    pcnt_config.pos_mode = PCNT_COUNT_INC;
    pcnt_config.neg_mode = PCNT_COUNT_DEC;
    pcnt_config.lctrl_mode = PCNT_MODE_REVERSE;
    pcnt_config.hctrl_mode = PCNT_MODE_KEEP;
    pcnt_config.counter_h_lim = Limit;
    pcnt_config.counter_l_lim = -Limit;

    pcnt_unit_config(&pcnt_config);
    pcnt_counter_pause(PCNT_UNIT_0);
    pcnt_counter_clear(PCNT_UNIT_0);
    pcnt_counter_resume(PCNT_UNIT_0);
}

int16_t EncoderSetCount()
{
    int16_t count = 0;
    pcnt_get_counter_value(PCNT_UNIT_0, &count);
    return count;
}
int EncoderClear()
{
    pcnt_counter_clear(PCNT_UNIT_0);
    return 0;
}
void EncoderPause()
{
    pcnt_counter_pause(PCNT_UNIT_0);
}

#pragma endregion

#pragma region PWM

void PWMInit(gpio_num_t Pin,  uint32_t Frequency, ledc_timer_bit_t DutyResolution, ledc_channel_t channelle)
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

void ConfigGPIO( gpio_num_t Pin)
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
void GPIOSetLevel( gpio_num_t Pin,  uint8_t Level)
{
    gpio_set_level(Pin, Level);
}

#pragma endregion

#pragma region Motor

void Motor::MotorAttached( gpio_num_t PWM,  gpio_num_t PinA,  gpio_num_t PinB, ledc_channel_t channel)
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