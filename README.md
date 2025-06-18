# Nexu_4WD

Ce projet consiste à refaire entièrement la carte de contrôle du robot Nexu 4WD, tout en conservant ses composants mécaniques (moteurs, capteurs, structure).  
La nouvelle carte intègre désormais le Bluetooth, ce qui permet de déplacer le robot via une application Android.  
Un ESP32 remplace l'ancien microcontrôleur, et plusieurs composants de contrôle ont été modifiés ou optimisés.

---

## Arborescence du projet

Le projet est divisé en plusieurs branches, correspondant à différentes parties :

- **Programme du robot** (C++ via ESP-IDF)
- **Application Android** (Kotlin via Android Studio)

---

### Programme du robot

- Pour accéder au programme global, consultez la branche `dev`.
- Pour accéder au code de chaque module séparément (moteurs, capteurs, Bluetooth...), consultez les branches dédiées à chaque composant.

---

### Application Android

- Le code de l’application Bluetooth Android est disponible dans la branche correspondante (`bluetooth_app` ou nom équivalent).

---

## Électronique

- Retrouvez les schémas électroniques (fichiers `.sch` et `.pcb`) et le routage dans la branche `hardware`.
- Le fichier Fusion 360 est également inclus pour modification mécanique.
- Pour plus de détails sur le montage de chaque composant, rendez-vous dans la branche ou le dossier correspondant à leur documentation.

---

## Composants de base du robot

Cette section contient :
- Les fiches techniques (datasheets) des composants utilisés.
- Des explications sur leur fonctionnement et leur intégration dans la nouvelle carte de control
