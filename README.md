# Nexu_4wd
Sur cette partie nous allons voir le programme prinsibale de notre robot, Pour ce programme nous avons plussieur lib : 
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
cette ligne ce mais dans le dans le 'void app_main'

### Fonctionement
#### envoyer un message
il faut changer la valuer du tableaux du 
