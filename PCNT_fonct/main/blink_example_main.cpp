
#include <stdio.h>
#include "rhoban_shell.h"
#include "lib/config.h"

#define PIN_Encoder_A GPIO_NUM_16
#define PIN_Encoder_B GPIO_NUM_17
#define PCNT_TEST_UNIT PCNT_UNIT_0
#define PCNT_H_LIM_VAL 100000

#define PinPWM GPIO_NUM_19
#define InA_1 GPIO_NUM_18
#define InA_2 GPIO_NUM_5

#define FrequencyMotor 30000
#define DutyResolutionMotor LEDC_TIMER_8_BIT

SHELL_PARAMETER_INT(CON, "Consigne", 5000);
SHELL_PARAMETER_INT(Speed, "Speed", 0);
SHELL_PARAMETER_FLOAT(kp, "KP", 1);
SHELL_PARAMETER_FLOAT(ki, "KI", 0);
SHELL_PARAMETER_FLOAT(kd, "KD", 0);
SHELL_PARAMETER_FLOAT(dt, "dt", 0);
SHELL_PARAMETER_INT(nb, "nb", 500);

float erreur, erreur_precedente = 0;
float integral = 0;
float vitesse_max = 6133.0;
float pwm_max = 254.0;

Motor motor;

float vittesse[5000];
int Speeds = 0;

float Vittesse()
{
    int count = EncoderClear();
    count = EncoderSetCount();
    vTaskDelay(((dt) * 1000) / portTICK_PERIOD_MS);
    count = EncoderSetCount();
    // ESP_LOGI("Count", "count : %d", count);
    float vitesse_moteur = (((count) * (60 / dt)) / 24);
    vitesse_moteur = vitesse_moteur;
    ESP_LOGI("Vitesse", "vitesse : %f", vitesse_moteur);
    return vitesse_moteur;
}

float PID(float consigne, float mesure, float dt)
{
    float vitesse_norm = mesure / vitesse_max;
    float consigne_norm = consigne / vitesse_max;
    float erreur = consigne_norm - vitesse_norm;
    integral += erreur * dt;
    float derivative = (erreur - erreur_precedente) / dt;
    float sortie = kp * erreur + ki * integral + kd * derivative;
    float pwm = sortie * pwm_max;
    ESP_LOGI("PID", "sortie : %f", sortie);
    erreur_precedente = erreur;
    return pwm;
}

void MotorVitesse(int fonction)
{
    int count = 0;

    switch (fonction)
    {
    case 0:
        Speeds = Speed;
        motor.SetSpeed(Speeds);
        break;
    case 1:
        for (int i = 0; i < nb; i++)
        {
            vittesse[i] = Vittesse();
            Speeds = PID(CON, vittesse[i], dt);
            motor.SetSpeed(Speeds);
        }

        ESP_LOGI("Vitesse", "fin de la messure");
        Speeds = 0;
        motor.SetSpeed(Speeds);
        break;
    case 2:
        for (int i = 0; i < nb; i++)
        {
            printf("%d : %f\n", i, vittesse[i]);
        }
        break;
    case 33:
        while (1)
        {
            count = EncoderSetCount();
            printf("count : %d\n", count);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        break;
    case 44:
        motor.SetSpeed(0);
        for (int i = 0; i < 255; i++)
        {
            motor.SetSpeed(i);
            vittesse[i] = Vittesse();
        }
        for (int i = 0; i < 255; i++)
        {
            printf("%d : %f\n", i, vittesse[i]);
        }
        motor.SetSpeed(0);
    default:
        break;
    }
}

extern "C"
{
    void app_main(void);
}
void app_main(void)
{
    shell_init(115200);
    shell_start_task();
    motor.MotorAttached(PinPWM, InA_1, InA_2, LEDC_CHANNEL_0);
    motor.MotorResolution(FrequencyMotor, DutyResolutionMotor);
    motor.InitMotor();
    motor.SetDirection(1);
    motor.SetSpeed(0);
    EncoderInit(PIN_Encoder_A, PIN_Encoder_B, PCNT_TEST_UNIT);
    /*while (1)
    {
        //dt = dtm/1000;
    }*/
}

SHELL_COMMAND(dir, "Set motor direction")
{
    if (argc < 1)
    {
        printf("Usage: dir <direction>\n");
        return;
    }
    int dir = atoi(argv[0]);
    MotorVitesse(dir);
}