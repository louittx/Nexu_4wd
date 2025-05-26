#include "lib/config.h"

uint8_t oldData = 0;
uint8_t NextData = 0;
void InitHc595(gpio_num_t dataPin, gpio_num_t clockPin, gpio_num_t latchPin)
{
    ConfigGPIO(dataPin);
    ConfigGPIO(clockPin);
    ConfigGPIO(latchPin);
}

void Hc595WriteByte(uint8_t data, gpio_num_t dataPin, gpio_num_t clockPin, gpio_num_t latchPin)
{
    uint8_t oldData = data;
    GPIOSetLevel(latchPin, 0);
    for (int i = 0; i < 8; i++)
    {
         GPIOSetLevel(clockPin, 0);
        GPIOSetLevel(dataPin, (data & (1 << (7 - i))) ? 1 : 0);
        GPIOSetLevel(clockPin, 1);
        vTaskDelay(1/ portTICK_PERIOD_MS);
    }
    GPIOSetLevel(latchPin, 1);
}
int DirHc595(int dir, bool sens,gpio_num_t dataPin, gpio_num_t clockPin, gpio_num_t latchPin)
{
    switch (dir)
    {
        case 0:
            if (sens)
            {
                NextData = oldData | 0b1;
            }
            else
            {
                NextData = oldData | 0b10;
            }
            break;
        case 1:
            if (sens)
            {
                NextData = oldData | 0b100;
            }
            else
            {
                NextData = oldData | 0b1000;
            }
            break;
        case 2:
            if (sens)
            {
                NextData = oldData | 0b10000;
            }
            else
            {
                NextData = oldData | 0b100000;
            }
            break;
        case 3:
            if (sens)
            {
                NextData = oldData | 0b1000000;
            }
            else
            {
                NextData = oldData | 0b10000000;
            }
            break;
    }
    return NextData;
}

#define PinDS GPIO_NUM_16
#define PinSTCP GPIO_NUM_4
#define PinSHCP GPIO_NUM_0

extern "C" void app_main()
{
    InitHc595(PinDS, PinSHCP, PinSTCP);
    while (true)
    {
        int dat = DirHc595(1,0, PinDS, PinSHCP, PinSTCP);
        ESP_LOGI("Hc595", "Data: %d", dat);
        Hc595WriteByte(dat, PinDS, PinSHCP, PinSTCP);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}