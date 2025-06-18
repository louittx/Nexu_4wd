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
cette ligne ce mais dans le dans le `void app_main`

### Fonctionement
#### Envoyer un message
Pour envoyer un message il faut modifier le `Buffer` et la `LengMessage`
le `Buffer` est un tableaux contenait les information d'un byte,
la `LengMessage` est un varaible qui permte de deffinr la taille de notre tableaux a envoyer
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
### Recuper un message
Pour reguper un message nous avons besoind de la variable `LengGetMessage` et tu tableaux `get_message`
le tableaux `get_message` permet d'avoir les information des byte envoyer
la variable `LengGetMessage` permet d'avoir le nombre de byte reçu ce qui intique la table de notre tableaux

**Exemple**
```
// get message = Hello
print("message reçu = ");
for (int i = 0; i<LengGetMessage, i++){
    print(get_message[i]);
}
print(\n);
```
