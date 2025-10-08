# MaYaRemote

MaYaRemote est un projet de télécommande universelle Infrarouge basé sur PlatformIO et la librairie IRremoteESP8266.
Le dispositif dispose d'une serveur bluetooth BLE pouvant se connecter à l'application MaYa (client Android) pour éxécuter des commandes.  

## ✨ Fonctionnalités
- Envoi de commandes **IR** (signaux infrarouges) prenant en charge une multitude de protocoles supportées par la librairie IRremote8266 (version actuelle : 2.8.6).
https://github.com/crankyoldgit/IRremoteESP8266
- Compatible avec le microcontrôleur ESP32 et d'autres (supporté par IRremoteESP8266).

## 📂 Structure du projet
Le code est organisé pour séparer la logique applicative et les drivers matériels.
src/
├── applicationInternal     # Couche application
├── hardware                # Couche matériel
├── interfaces              # Interfaces entres les couches
├── helpers                 # Outils d'aide au codage
├── utils                   # Code utilitaires
platformio.ini              # Configuration du projet PlatformIO

## 📦 Modules et classes programme
- Surcouche de communication Blutooth BLE avec gestion de la réponse et des timeouts.
- Classe de découpage des messages json en paquets permettant de lever la limitation BLE.
- Surcouche de gestion des systèmes de fichiers LittleFS et SPIFFS.
- Classe de mise en fil d'attente pour exécution de commandes asynchrones.
- Surcouche de reception des signaux infrarouge.
- Surcouche d'emission des signaux infrarouge (avec prise en charge de paramétres additionnelles).
- Classe pour délai de prise en compte avant exécution des commandes lors d'un appui sur un bouton.
- Classe de capture IR avec stockage du resultat en mémoire.
- Classe de Mise en veille/Reveille du microcontroleur.
- Classe Clignoteur pour Led
- Calcul du tickRate serveur.

## 🔧 Prérequis
- [PlatformIO](https://platformio.org/) installé (dans VS Code ou en CLI).
- L'application est dévelllopé en C++11 (version par défaut prise en charge par le framework Arduino)
- Cartes supportées ESP32, ESP8266 (espressif32), Pico (raspberrypi)... :
_________________________________________________________________________________________
| Plateforme  | Carte (nom commercial)              | Carte (support logiciel)          |
|-------------|-------------------------------------|-----------------------------------|
| espressif32 | Carte générique ESP32               | esp32-s3-devkitc-1                |
| espressif32 | freenove_esp32s3_wroom1             | esp32-s3-devkitc-1                |
| espressif32 | dfrobot_firebeetle2_esp32e_dfr0654  | esp32dev                          |
| espressif32 | dfrobot_firebeetle2_esp32e_dfr1139  | esp32dev                          |
| espressif32 | dfrobot_firebeetle2_esp32s3_dfr0975 | dfrobot_firebeetle2_esp32s3       |
| raspberrypi | pico                                | pico                              |
_________________________________________________________________________________________

- Capteur / émetteur IR compatible:
Emetteur infrarouge 5mm 940nm 100mA +/-30° OSRAM SFH4547 (ou équivalent)
Recepteur infrarouge 38kHz VS1838 (ou équivalent)

## 🚀 Compilation & Flash
1. Cloner le dépôt sur VS Code:
    https://github.com/Chris33240/MaYaRemote3.git

2. Connecter la carte au port USB du PC de develloppement.
    (Veuillez vérifier que le pilote du contrôleur USB est correctement installé sur l’ordinateur.).

3. Graphe des dépendances:
|-- IRremoteESP8266 @ 2.8.6
|-- ArduinoJson @ 7.3.0
|-- ESP32 BLE Arduino @ 2.0.0
|-- Wire @ 2.0.0
|-- FS @ 2.0.0
|-- LittleFS @ 2.0.0
|-- SPIFFS @ 2.0.0

4. Ressources:
Exemple de ressources utilisées pour la carte 'dfrobot_firebeetle2_esp32e_dfr0654'
board_build.partitions = huge_app.csv
HARDWARE: ESP32 240MHz, 320KB RAM, 4MB Flash
RAM:   [=         ]  12.5% (used 40932 bytes from 327680 bytes)
Flash: [=====     ]  48.1% (used 1512017 bytes from 3145728 bytes)

