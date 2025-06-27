# Nexu_4WD

Sur cette partie, nous allons voir l'aplication android et sont fo,ctionnement, je vais aussi vous expliquer la programmation

Pour téleverser l'aplication j'ai utiliser andrad studio code qui permet de théléchadre l'aplication sur télephone grace au programme.

L'aplication ce passe sur plusssieur 4 page : 
- MainActivity1 : page d'acuille
- MainActivity2 : page de connextion au apariel Bluetooth
- MainActivity3 : page de controle du robot
- MainActivity4 : page de paramatre

## MainActivity1
Sur cette page nous 3 bouton : 
- 1er bouton : Change de page pour accéter au MainActivity2
- 2emme bouton : cahnge de page pour acceter au MainActivity3, cela peux etre activier que si nous somme connecter a un appareil bluetooth sino affiche un message
- 3emem bouton : changer de page pour aller au MainActivity4
### Changement de page 
```
```
**Exemple**
```
```

## MainActivity2
Sur cette page nous avosn une listView avec 2 bouton
- **listeView** : cette liste contiintra plusssieur action la premier c'est l'affichage des nom des apareil bluetooth apareil au thélephone, SUr cette meme liste nous pouront cliker sur chaque element se qui nous prematera d'essayer de nous conneter a l'apparile bluetooth, cela va aussi afficher un image qui montrera que nous essayer de nous conneter. Quand nous clikons nous avons un toast qui s'afficher pour nous afficher el noms de noter apperil bluetooth ou nous avosn cliker ensuis il affichera un autre toast pour affichre si la connextion est activer ou pas.
- **bouton** : le 1er bouton est le bouton refrech qui permte de réactualisre al listView a afficher l'apparil connxter au pas grace a la bar vert, Pour le 2emme bouton est le bouton deconection, ce bouton est clikable que si nous somme conecter a un apraiels, il premtra de ce deconnecter de notre appareil.
### Recuparation des apperile appareil
```
```
**Exemple**
```
```

## MainActivity3 
Sur cette page nous avons une partre controle du robot et un autre partie reception :

Partie **Controle** : sur cette paretie nous avosn 2 controleur, le premier et le jostyique qui permet de controler la vittesse de notre robot et de controller ca direction sens rotation, esuit nous avons une **bar de progression** qui permetra de controler la rotation de notre robot. 

Partie **recption** : Sur cette partie nous avosn 7 **bar**, 4 bar permet de visualiser la vittesse de chaque roue de notre robot, le rest permet de recuper les information de la distance des 3 cpateur de distance sur notre robot.

Pour le foonctionnement de cette page, nous avons 4 partie :
- Rececption des donnée bluetooth
- Envoer des donnée bluetooth
- Recuperation les information de control
- Affichage des information
- 
### Recuperation les information de control
### Affichage des information

## Service Bluetooth
Pour la conextion en bluetooth j'ai crées un service, un service c'est un page qui se lance en arrier plans, ce qui me permet d'avoir toujour les information des donner du bluetooth comme l'apperile apperile. Ce service a plusssieur fonction qui permte al comunication en bluetooth et la connextion. Pour utliser ce service il font **l'initialiser**
```
```
Pour lancer le pharse des fonctione de ce service il faut bien etre connecter au service sinon cela fait crash( atternde 1 seconde par exmple).
### Reception des données Bluetooth
Pour le reception de donner Nous utlision 
```
```
**Exemple**
```
```
### Envoyer des données Bluetooth
Pour l'envoyer des donner Nous utlision
```
```
**Exemple**
```
```
