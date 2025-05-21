#include <lib/RS485.h>

#define ECHO_TASK_STACK_SIZE (CONFIG_ECHO_TASK_STACK_SIZE)
#define ECHO_TASK_PRIO          (10)

char DataReceive[126];
Sensor s1;

int length = 0;

static void RS485(void *arg){
    InitRS485();
    s1.SensorAdress(0x12);
    while(1){
        s1.InitSensorTrigger();
        vTaskDelay(70 / portTICK_PERIOD_MS);
        //length = s1.GetDistance();
        length = s1.GetTemp();
    }
}
extern "C"
{
    void app_main(void);
}
void app_main(void){
    length = 0;
    xTaskCreate(RS485, "RS485", ECHO_TASK_STACK_SIZE, NULL, ECHO_TASK_PRIO, NULL);
    while(1){
        ESP_LOGI("RS485", " value temp: %f", s1.ValueTemp(DataReceive));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}