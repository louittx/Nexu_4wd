
#include <stdio.h>
#include "rhoban_shell.h"
#include "lib/config.h"

#define PIN_Encoder_A GPIO_NUM_26
#define PIN_Encoder_B GPIO_NUM_25
#define PCNT_TEST_UNIT PCNT_UNIT_0
#define PCNT_H_LIM_VAL 100000

#define PinPWM GPIO_NUM_19
#define InA_1 GPIO_NUM_18
#define InA_2 GPIO_NUM_5

#define FrequencyMotor 30000
#define DutyResolutionMotor LEDC_TIMER_8_BIT

SHELL_PARAMETER_INT(CON, "Consigne", 5000);
SHELL_PARAMETER_FLOAT(kp, "KP", 1);
SHELL_PARAMETER_FLOAT(ki, "KI", 3);
SHELL_PARAMETER_FLOAT(kd, "KD", 0.0015);

float erreur, erreur_precedente = 0;
float integral = 0;
float vitesse_max = 7300.0;
float pwm_max = 254.0;
float dt = 0.01;

MotorEncoder motorEncoder;

float vittesse[5000];
int Speeds = 0;


extern "C"
{
    void app_main(void);
}
void app_main(void)
{
    shell_init(115200);
    shell_start_task();
    motorEncoder.MotorAttached(PinPWM, InA_1, InA_2, LEDC_CHANNEL_0);
    motorEncoder.MotorResolution(FrequencyMotor, DutyResolutionMotor);
    motorEncoder.EncodeurAttached(PIN_Encoder_A, PIN_Encoder_B, PCNT_TEST_UNIT, PCNT_TEST_UNIT);
    motorEncoder.InitMotorEncodeur();
    motorEncoder.SetDirection(1);
    motorEncoder.SetSpeed(0);
    EncoderInit(PIN_Encoder_A, PIN_Encoder_B, PCNT_TEST_UNIT, PCNT_TEST_UNIT);
    //MotorVitesse(44);
    while (1)
    {
     motorEncoder.SetSpeedPID(CON,-1, kp, ki, kd);
     vTaskDelay(10/portTICK_PERIOD_MS);   
    }
}

/*SHELL_COMMAND(dir, "Set motor direction")
{
    if (argc < 1)
    {
        printf("Usage: dir <direction>\n");
        return;
    }
    int dir = atoi(argv[0]);
    MotorVitesse(dir);
}*/