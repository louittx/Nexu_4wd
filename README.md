# Nexu_4WD

Sur cette partie, nous allons voir le programme principal de notre robot.  
Pour ce programme, nous avons plusieurs librairies :

- **Bt** : permet la communication en Bluetooth  
- **Config** : permet de configurer tous les éléments moteurs du robot  
- **RS485** : permet la communication en RS485  

## Bluetooth

### Variables à définir

Les variables sont à intégrer dans le code pour le fonctionnement de la librairie **Bt** :

```
u_int8_t get_message[16];
uint8_t LengGetMessage;
int VM[4];
int Capteur[3];
char Buffer[1023];
bool SendMessage;
uint8_t LengMessage;
```

### Initialisation

À mettre dans le `void app_main()` :

```
Init();
```

### Fonctionnement

#### Envoyer un message

Pour envoyer un message, il faut modifier le `Buffer` et `LengMessage` :
- `Buffer` est un tableau contenant les informations en bytes.
- `LengMessage` est une variable qui permet de définir la taille du tableau à envoyer.

**Exemple** :

```c
// sendMsg = Hello
Buffer[0] = 'H';
Buffer[1] = 'e';
Buffer[2] = 'l';
Buffer[3] = 'l';
Buffer[4] = 'o';
LengMessage = 5;
```

#### Récupérer un message

Pour récupérer un message, on utilise `LengGetMessage` et le tableau `get_message` :
- `get_message` contient les données reçues (bytes).
- `LengGetMessage` indique la taille du message reçu.

**Exemple** :

```c
// get message = Hello
printf("message reçu = ");
for (int i = 0; i < LengGetMessage; i++) {
    printf("%c", get_message[i]);
}
printf("\n");
```

## Config

Cette librairie contient 3 classes :

- **Motor** : contrôle du moteur avec PWM et sens de rotation  
- **MotorEncoder** : ajoute la lecture de la vitesse, du sens, et un asservissement PID  
- **MotorEncoderHC595** : ajoute en plus la gestion du sens via un 74HC595 (jusqu'à 4 moteurs)  

### Variable à définir

```
int Data;
```

### Initialisation

```c
MotorAttached(gpio_num_t PWM, ledc_channel_t channel, uint8_t motor);
MotorResolution(uint32_t Frequency, ledc_timer_bit_t DutyResolution);
EncodeurAttached(gpio_num_t PinA, gpio_num_t PinB, pcnt_unit_t PcntUnit, uint64_t limit);
hc595Attached(gpio_num_t dataPin, gpio_num_t clockPin, gpio_num_t latchPin);
```

- `PWM` : broche PWM pour contrôler la puissance du moteur  
- `PinA` et `PinB` : broches de l’encodeur  

Après avoir tout initialisé, lancez :

```c
InitMotorEncodeurHC595();
```

### Fonctionnement

#### Set

```c
SetSpeed(int Speed);
```

Permet de définir la puissance du moteur via le PWM.

```c
SetSpeedPID(int consigne, float Speeds, float Kp, float Ki, float Kd);
```

Permet de faire tourner le moteur à une vitesse donnée en tr/min avec régulation PID.

**Paramètres :**
- `consigne` : vitesse cible (tr/min, sans réducteur)
- `Speeds` : mettre à -1 pour laisser la fonction calculer automatiquement
- `Kp`, `Ki`, `Kd` : coefficients PID

```c
Hc595WriteByte(uint8_t data);
```

Met à jour les sorties du registre 74HC595. Si plusieurs moteurs sont connectés, un seul appel suffit.

**Exemple** :

```c
// Le moteur tourne à 7000 tr/min puis à 50 % dans l’autre sens
Motor.SetSpeedPID(7000, -1, 1, 2, 0.001);
vTaskDelay(pdMS_TO_TICKS(100));
Data = Motor.DirHc595(2); // voir explication ci-dessous
Motor.Hc595WriteByte(Data);
Motor.SetSpeed(50);
vTaskDelay(pdMS_TO_TICKS(100));
```

#### Get

```c
float Speed = SpeedMotor();
```

Renvoie la vitesse du moteur (tr/min).

```c
Data = DirHc595(int dir);
```

Définit le sens de rotation du moteur sans affecter les autres.

## RS485

Cette librairie permet la communication RS485 avec les capteurs ultrasoniques et de température.

### Variable à définir

```
char DataReceive[126];
```

### Initialisation

```c
SensorAdress(uint8_t Address);
```

Permet d'initialiser un capteur à une adresse donnée.

### Distance

```c
Sensor.InitSensorTrigger();
```

Configure le trigger du capteur ultrason.

```c
int len = Sensor.GetDistance();
```

Envoie une requête et attend la réponse (longueur du message).

```c
int dist = Sensor.ValueDistance();
```

Renvoie la distance mesurée.

**Exemple** :

```c
Sensor.InitSensorTrigger();
vTaskDelay(pdMS_TO_TICKS(70));
int len = Sensor.GetDistance();
int dist = Sensor.ValueDistance();
printf("distance = ");
printf("%d", dist);
printf("\n");
```

### Température

```c
int len = Sensor.GetTemp();
```

Envoie une demande de température et lit la longueur de la réponse.

```c
float temp = Sensor.ValueTemp(DataReceive);
```

Retourne la température en degrés Celsius.

**Exemple** :

```c
int len = Sensor.GetTemp();
float temp = Sensor.ValueTemp(DataReceive);
printf("Température = %f°C\n", temp);
```
