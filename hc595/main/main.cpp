#include "lib/config.h"

int Data = 0;


#define PinDS GPIO_NUM_16
#define PinSTCP GPIO_NUM_4
#define PinSHCP GPIO_NUM_0

MotorEncoderHc595 motorEncoderHc595;
MotorEncoderHc595 motorEncoderHc5952;


extern "C" void app_main()
{
    motorEncoderHc595.hc595Attached(PinDS, PinSHCP, PinSTCP);
    motorEncoderHc595.MotorAttached(GPIO_NUM_2, LEDC_CHANNEL_0, 0);
    motorEncoderHc595.MotorResolution(1000, LEDC_TIMER_10_BIT);
    motorEncoderHc595.EncodeurAttached(GPIO_NUM_5, GPIO_NUM_18, PCNT_UNIT_0, 10000);
    motorEncoderHc595.InitMotorEncodeurHC595();
    
    motorEncoderHc5952.hc595Attached(PinDS, PinSHCP, PinSTCP);
    motorEncoderHc5952.MotorAttached(GPIO_NUM_2, LEDC_CHANNEL_0, 1);
    motorEncoderHc5952.MotorResolution(1000, LEDC_TIMER_10_BIT);
    motorEncoderHc5952.EncodeurAttached(GPIO_NUM_5, GPIO_NUM_18, PCNT_UNIT_0, 10000);
    motorEncoderHc5952.InitMotorEncodeurHC595();

    Data = motorEncoderHc5952.DirHc595(2);
    Data = motorEncoderHc595.DirHc595(2);
    motorEncoderHc595.Hc595WriteByte(Data);
    ESP_LOGI("MotorEncoderHc595", "Data sent to HC595: %d", Data);

}