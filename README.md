# Nexu_4wd
Sur cette partie nous allons voir le programme prinsibale de notre robot, Pour ce programme nous avons plussieur lib : 
- Bt : cette lib permet la comunictaion en bluetooth
- config : cette lib permet de configuret tout les element des moteur de notre robot
- RS485 : cette lib permet la comunication en RS485

## Bluetooth
```
extern u_int8_t get_message[16];
extern uint8_t LengGetMessage;
extern int VM[4];
extern int Capteur[3];
extern char Buffer[1023];
extern bool SendMessage;
extern uint8_t LengMessage;
```