# Nexu_4wd
Sur cette partie nous allons voir le programme prinsibale de notre robot, Pour ce programme nous avons plussieur lib: 
- Bt : cette lib permet la comunictaion en bluetooth
- config : cette lib permet de configuret tout les element des moteur de notre robot
- RS485 : cette lib permet la comunication en RS485

## Bluetooth
### variable a definir
Les variable sont a intégre dans le code pour le fonctionnement de la lib bt
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
```
Init()
```
cette ligne ce mais dans le dans le `void app_main`

### Fonctionement
#### Envoyer un message
Pour envoyer un message il faut modifier le `Buffer` et la `LengMessage`.
- le `Buffer` est un tableaux contenait les information d'un byte.
- la `LengMessage` est un varaible qui permte de deffinr la taille de notre tableaux a envoyer.
**Exemple**
```
// sendMsg = Hello
Buffer[0] = 'H';
Buffer[1] = 'e';
Buffer[2] = 'l';
Buffer[3] = 'l';
Buffer[4] = 'o';
LengMessage = 5;
```
#### Recuper un message
Pour reguper un message nous avons besoind de la variable `LengGetMessage` et tu tableaux `get_message`.
- le tableaux `get_message` permet d'avoir les information des byte envoyer.
- la variable `LengGetMessage` permet d'avoir le nombre de byte reçu ce qui intique la table de notre tableaux.

**Exemple**
```
// get message = Hello
print("message reçu = ");
for (int i = 0; i<LengGetMessage, i++){
    print(get_message[i]);
}
print(\n);
```

## Config
Sur cette lib il y a 3 `class`
- Motor : permet de controler les moteur aevc le PWM et sont sens
- MotorEncoder : Prendre les même fonction que Motor en rajoutant de fonction pour obtenir la vittesse, le sens et rajoute un aservisement PID
- MotorEncoderHC595 ; Prend les même fonction que MotorEncoder en rajouter le changemeent de sens par un 74HC595 pour les moteur max 4 moteur par 74HC595

### variable a definir
Les variable sont a intégre dans le code pour le fonctionnement de la lib bt
```
int Data;
```
### Initialisation
```
MotorAttached(gpio_num_t PWM, ledc_channel_t channel, uint8_t motor);
MotorResolution(uint32_t Frequency, ledc_timer_bit_t DutyResolution);
EncodeurAttached(gpio_num_t PinA, gpio_num_t PinB, pcnt_unit_t PcntUnit, uint64_t limit);
hc595Attached(gpio_num_t dataPin, gpio_num_t clockPin, gpio_num_t latchPin);
```
`PWM` = Pin PWM pour controller la puissant de notre moteur, `PinA` et `PinB` = Pin de l'encodeur
Apres avoir lancer tout c'est il faut initialise avec : 
```
InitMotorEncodeurHC595();
```

### Fonctionement 
#### Set
```
SetSpeed(int Speed);
```
cette fonction permet de selectionner la puissant de notre moteur selon la valuer du PWM

```
SetSpeedPID(int consigne, float Speeds, float Kp, float Ki, float Kd);
```
cette fonction permet de faire tourne notre moteur a vittesse donner en tr/min garce a un PID et cela permetra de corriger le vittesse si le robot donc plus forcer ou pas, cette fonctionn resoit plussieur parametre : 
- `consigne` : cela permte de dire notre vittesse ou nous voudrons que notre moteur sans reducteur tourne
- `Speeds` : Si il est a -1 alors dans la fonctionn notre moteur calculera sa vittesse tout seul, sont c'est egal a la vittesse de notre moteur mesurer
- `Kp`, `Ki` et `Kd` ; cela permet de mettre le coefisant pour le PID
```
Hc595WriteByte(uint8_t data);
```
cette fonctionne permte d'actualiser le register a le 74HC595, donc si nous avons une modification pour sur le sens elle ce fera que quand nous alons actualiser le 74HC595. Si nous avosn 4 moteur cette fonction peux etre appler 1 fois pour les 4 moteur

**Exemple**
```
// ont veux que le moteur tounre a 7000 tr/min puis a 50% dans l'autre sens
Motor.SetSpeedPID(7000,-1,1,2,0.001);
vTaskDelay(pdMS_TO_TICKS(100));
Data = Motor.DirHc595(2); // et expliquer en dessous
Motor.Hc595WriteByte(Data);
SetSpeed(50);
Motor.vTaskDelay(pdMS_TO_TICKS(100));

```
#### Get
```
flaot Speed = SpeedMotor();
```
Cette fonctionne permet d'obtenir la vittesse de notre moteur(tr/min) sans reducteur
```
Data = DirHc595(int dir);
```
cette fonction permte de definir le sens de notre moteur sans changer le sens des 4 autre moteur

##RS485
Cette lib permet la cominication en RS485 pour les capteur ultrason et de temperature
### variable a definir
```
char DataReceive[126];
```
### Initialisation
```
SensorAdress(uint8_t Address);
```
Initalise le capetur

## Fonction
### Distance
```
InitSensorTrigger();
```
permte de set le trigger

```
int len = GetDistance
```
permet d'envoyer le message pour resevoir puis reçois le message. revoir la lenguer du message reçus

```
int dist = ValueDistance
```
permte de verifier si le message revoit bien le distance et revoir la distance si cela et jsute revois la valuer de la distance.
