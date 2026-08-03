/// @file ble_server_hal_esp32.cpp
/// @brief Serveur BLE pour ESP32 avec plusieurs services et caractéristiques.
///
/// Video: https://www.youtube.com/watch?v=oCMOYS71NIU
/// Basé sur l'exemple de Neil Kolban et porté sur Arduino ESP32 par Evandro Copercini :
/// https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
/// Permet de créer un serveur BLE capable de gérer les notifications périodiques,
/// la réception de commandes, et l'envoi d'informations système.
///
/// Le principe pour créer un serveur BLE :
///   1. Create a BLE Server
///   2. Create a BLE Service
///   3. Create a BLE Characteristic on the Service
///   4. Create a BLE Descriptor on the characteristic
///   5. Start the service.
///   6. Start advertising.
///
/// @note Service System Infos: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
/// @note RX Characteristic (WRITE): 6E400002-B5A3-F393-E0A9-E50E24DCCA9E
/// @note TX Characteristic (NOTIFY): 6E400003-B5A3-F393-E0A9-E50E24DCCA9E
#include "ble_server_hal_esp32.h"
#include <HardwareSerial.h>
#include "ble_uuid.h"
#include "ble_callback_system_infos.h"
#include "ble_callback_commands.h"
#include "ble_callback_uart.h"
#include "system_infos.h"
#include "tick_rate.h"

/// @brief Pointeur vers le serveur BLE
BLEServer *pMyServer = nullptr;

/// @brief Service System Infos
BLEService *pService_SystemInfos = nullptr;
BLECharacteristic *pCharacteristic_READ_CPU_CHIP_MODEL = nullptr;
BLECharacteristic *pCharacteristic_READ_CPU_CHIP_REVISION = nullptr;
BLECharacteristic *pCharacteristic_READ_CPU_FREQUENCY = nullptr;
BLECharacteristic *pCharacteristic_READ_FREE_HEAP = nullptr;
BLECharacteristic *pCharacteristic_READ_TOTAL_HEAP = nullptr;
BLECharacteristic *pCharacteristic_READ_HWATER_MARK = nullptr;
BLECharacteristic *pCharacteristic_READ_MIN_FREE_HEAP = nullptr;
BLECharacteristic *pCharacteristic_READ_MAX_ALLOC_HEAP = nullptr;
BLECharacteristic *pCharacteristic_NOT_TICK_RATE = nullptr;
BLECharacteristic *pCharacteristic_READ_USED_BYTES = nullptr;
BLECharacteristic *pCharacteristic_READ_TOTAL_BYTES = nullptr;

/// @brief Service Commands
BLEService *pService_Commands = nullptr; // commands + notify code
BLECharacteristic *pCharacteristic_READ_COMMANDS_COUNT = nullptr;
BLECharacteristic *pCharacteristic_READ_COMMANDS_FILES_COUNT = nullptr;
BLECharacteristic *pCharacteristic_READ_LIST_COMMANDS = nullptr;
BLECharacteristic *pCharacteristic_READ_LIST_COMMANDS_DATA = nullptr;
BLECharacteristic *pCharacteristic_READ_LAST_CAPTURE = nullptr;
// BLECharacteristic *pCharacteristic_WRITE_COMMAND = nullptr;
BLECharacteristic *pCharacteristic_WRITE_COMMAND2 = nullptr;
BLECharacteristic *pCharacteristic_WRITE_EXECUTE_COMMAND = nullptr;
BLECharacteristic *pCharacteristic_WRITE_DELETE_COMMAND = nullptr;
BLECharacteristic *pCharacteristic_NOTIFY_COMMANDS = nullptr;
BLECharacteristic *pCharacteristic_NOTIFY_NEW_COMMAND_CAPTURED = nullptr;
BLECharacteristic *pCharacteristic_NOT_Test = nullptr;

/// @brief Service Notify
BLEService *pService_Notify = nullptr; // Notify message
BLECharacteristic *pCharacteristic_Notify = nullptr;

/// @brief Service UART
BLEService *pService_UART = nullptr;
BLECharacteristic *pRxCharacteristic_UART = nullptr;

/// @brief Pointeur vers l'objet publicité BLE
BLEAdvertising *pAdvertising = nullptr;

/// @brief Indicateurs de connexion client
bool deviceConnected = false;
bool oldDeviceConnected = false;

/// @brief Valeur transmise périodiquement
uint8_t txValue = 0;

/// @brief Gestion du timing des notifications
unsigned long startTimeNotify = 0;
unsigned long *pStartTimeNotify = &startTimeNotify;
const long intervalNotify = 1000; // Intervalle de 1 seconde
const long *pIntervalNotify = &intervalNotify;

// ------------------- Init serveur BLE ---------------------------
/// @brief Initialise le serveur BLE, les services et les caractéristiques
void init_ble_server_HAL()
{
    Serial.println(F("BLE starting"));

    // Initialiser le Device BLE
    BLEDevice::init(BLE_DEVICE_NAME);
    // BLEDevice::setMTU(512); // Need to be negociated with client ?!
    Serial.print(F("BLE device name: "));
    Serial.println(BLE_DEVICE_NAME);

    // Créer le serveur BLE
    pMyServer = BLEDevice::createServer();
    pMyServer->setCallbacks(new MyServerCallbacks());

    // ------------- SERVICE SYSTEM INFOS --------------------------
    // Créer le service BLE
    // Each characteristic needs 2 handles and descriptor 1 handle
    pService_SystemInfos = pMyServer->createService(BLEUUID(SERVICE_SYSTEM_INFOS_UUID), 37); // Don't forget to change handles count (default = 15)

    pCharacteristic_READ_CPU_CHIP_MODEL = pService_SystemInfos->createCharacteristic(
        CHARACTERISTIC_CPU_CHIP_MODEL_UUID,
        BLECharacteristic::PROPERTY_READ);
    pCharacteristic_READ_CPU_CHIP_MODEL->setCallbacks(new MyCallbacksSystemInfos());

    pCharacteristic_READ_CPU_CHIP_REVISION = pService_SystemInfos->createCharacteristic(
        CHARACTERISTIC_CPU_CHIP_REVISION_UUID,
        BLECharacteristic::PROPERTY_READ);
    pCharacteristic_READ_CPU_CHIP_REVISION->setCallbacks(new MyCallbacksSystemInfos());

    pCharacteristic_READ_CPU_FREQUENCY = pService_SystemInfos->createCharacteristic(
        CHARACTERISTIC_CPU_FREQUENCY_UUID,
        BLECharacteristic::PROPERTY_READ);
    pCharacteristic_READ_CPU_FREQUENCY->setCallbacks(new MyCallbacksSystemInfos());

    pCharacteristic_READ_FREE_HEAP = pService_SystemInfos->createCharacteristic(
        CHARACTERISTIC_FREE_HEAP_UUID,
        BLECharacteristic::PROPERTY_READ);
    pCharacteristic_READ_FREE_HEAP->setCallbacks(new MyCallbacksSystemInfos());

    pCharacteristic_READ_TOTAL_HEAP = pService_SystemInfos->createCharacteristic(
        CHARACTERISTIC_TOTAL_HEAP_UUID,
        BLECharacteristic::PROPERTY_READ);
    pCharacteristic_READ_TOTAL_HEAP->setCallbacks(new MyCallbacksSystemInfos());

    pCharacteristic_READ_HWATER_MARK = pService_SystemInfos->createCharacteristic(
        CHARACTERISTIC_HWATER_MARK_UUID,
        BLECharacteristic::PROPERTY_READ);
    pCharacteristic_READ_HWATER_MARK->setCallbacks(new MyCallbacksSystemInfos());

    pCharacteristic_READ_MIN_FREE_HEAP = pService_SystemInfos->createCharacteristic(
        CHARACTERISTIC_MIN_FREE_HEAP_UUID,
        BLECharacteristic::PROPERTY_READ);
    pCharacteristic_READ_MIN_FREE_HEAP->setCallbacks(new MyCallbacksSystemInfos());

    pCharacteristic_READ_MAX_ALLOC_HEAP = pService_SystemInfos->createCharacteristic(
        CHARACTERISTIC_MAX_ALLOC_HEAP_UUID,
        BLECharacteristic::PROPERTY_READ);
    pCharacteristic_READ_MAX_ALLOC_HEAP->setCallbacks(new MyCallbacksSystemInfos());

    pCharacteristic_NOT_TICK_RATE = pService_SystemInfos->createCharacteristic(
        CHARACTERISTIC_TICK_RATE_UUID,
        BLECharacteristic::PROPERTY_NOTIFY);
    pCharacteristic_NOT_TICK_RATE->addDescriptor(new BLE2902()); // Ajouter un descripteur pour la notification

    pCharacteristic_READ_USED_BYTES = pService_SystemInfos->createCharacteristic(
        CHARACTERISTIC_USED_BYTES_UUID,
        BLECharacteristic::PROPERTY_READ);
    pCharacteristic_READ_USED_BYTES->setCallbacks(new MyCallbacksSystemInfos());

    pCharacteristic_READ_TOTAL_BYTES = pService_SystemInfos->createCharacteristic(
        CHARACTERISTIC_TOTAL_BYTES_UUID,
        BLECharacteristic::PROPERTY_READ);
    pCharacteristic_READ_TOTAL_BYTES->setCallbacks(new MyCallbacksSystemInfos());

    // ------------- SERVICE COMMANDS --------------------------
    pService_Commands = pMyServer->createService(BLEUUID(SERVICE_COMMANDS_UUID), 24);

    pCharacteristic_READ_COMMANDS_COUNT = pService_Commands->createCharacteristic(
        CHARACTERISTIC_COMMANDS_COUNT_UUID,
        BLECharacteristic::PROPERTY_READ);
    addDescriptor(pCharacteristic_READ_COMMANDS_COUNT, CHARACTERISTIC_COMMANDS_COUNT_DESCRIPTOR_UUID, "Nombres de commandes");
    pCharacteristic_READ_COMMANDS_COUNT->setCallbacks(new MyCallbacks());

    pCharacteristic_READ_COMMANDS_FILES_COUNT = pService_Commands->createCharacteristic(
        CHARACTERISTIC_COMMANDS_FILES_COUNT_UUID,
        BLECharacteristic::PROPERTY_READ);
    pCharacteristic_READ_COMMANDS_FILES_COUNT->setCallbacks(new MyCallbacks());

    pCharacteristic_READ_LIST_COMMANDS = pService_Commands->createCharacteristic(
        CHARACTERISTIC_LIST_COMMANDS_UUID,
        BLECharacteristic::PROPERTY_READ);
    pCharacteristic_READ_LIST_COMMANDS->setCallbacks(new MyCallbacks());

    pCharacteristic_READ_LIST_COMMANDS_DATA = pService_Commands->createCharacteristic(
        CHARACTERISTIC_LIST_COMMANDS_DATA_UUID,
        BLECharacteristic::PROPERTY_READ);
    pCharacteristic_READ_LIST_COMMANDS_DATA->setCallbacks(new MyCallbacks());

    pCharacteristic_READ_LAST_CAPTURE = pService_Commands->createCharacteristic(
        CHARACTERISTIC_LAST_CAPTURE_UUID,
        BLECharacteristic::PROPERTY_READ);
    pCharacteristic_READ_LAST_CAPTURE->setCallbacks(new MyCallbacks());

    pCharacteristic_WRITE_COMMAND2 = pService_Commands->createCharacteristic(
        CHARACTERISTIC_COMMAND2_UUID,
        BLECharacteristic::PROPERTY_WRITE);
    pCharacteristic_WRITE_COMMAND2->setCallbacks(new MyCallbacks());

    pCharacteristic_WRITE_EXECUTE_COMMAND = pService_Commands->createCharacteristic(
        CHARACTERISTIC_EXECUTE_COMMAND_UUID,
        BLECharacteristic::PROPERTY_WRITE);
    pCharacteristic_WRITE_EXECUTE_COMMAND->setCallbacks(new MyCallbacks());

    pCharacteristic_WRITE_DELETE_COMMAND = pService_Commands->createCharacteristic(
        CHARACTERISTIC_DELETE_COMMAND_UUID,
        BLECharacteristic::PROPERTY_WRITE);
    pCharacteristic_WRITE_DELETE_COMMAND->setCallbacks(new MyCallbacks());

    pCharacteristic_NOTIFY_COMMANDS = pService_Commands->createCharacteristic(
        CHARACTERISTIC_NOTIFY_COMMANDS_UUID,
        BLECharacteristic::PROPERTY_NOTIFY);
    pCharacteristic_NOTIFY_COMMANDS->addDescriptor(new BLE2902()); // Ajouter un descripteur pour la notification

    pCharacteristic_NOTIFY_NEW_COMMAND_CAPTURED = pService_Commands->createCharacteristic(
        CHARACTERISTIC_NOTIFY_NEW_COMMAND_CAPTURED_UUID,
        BLECharacteristic::PROPERTY_NOTIFY);
    pCharacteristic_NOTIFY_NEW_COMMAND_CAPTURED->addDescriptor(new BLE2902()); // Ajouter un descripteur pour la notification

    pCharacteristic_NOT_Test = pService_Commands->createCharacteristic(
        CHARACTERISTIC_TEST_UUID_NOT,
        BLECharacteristic::PROPERTY_NOTIFY);
    pCharacteristic_NOT_Test->addDescriptor(new BLE2902()); // Ajouter un descripteur pour la notification

    // ------------- SERVICE Notify --------------------------
    // Créer le service BLE
    pService_Notify = pMyServer->createService(SERVICE_NOTIFY_UUID);

    pCharacteristic_Notify = pService_Notify->createCharacteristic(
        CHARACTERISTIC_NOTIFY_UUID,
        BLECharacteristic::PROPERTY_NOTIFY);
    pCharacteristic_Notify->addDescriptor(new BLE2902()); // Ajouter un descripteur pour la notification

    // ------------- SERVICE UART --------------------------
    pService_UART = pMyServer->createService(SERVICE_UART_UUID);

    pRxCharacteristic_UART = pService_UART->createCharacteristic(
        CHARACTERISTIC_UART_UUID_RX,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY);
    pRxCharacteristic_UART->addDescriptor(new BLE2902());
    pRxCharacteristic_UART->setCallbacks(new MyCallbacks_UART());
    // ----------------------------------------------------------

    // Démarrer le service
    pService_SystemInfos->start();
    pService_Commands->start();
    pService_Notify->start();
    pService_UART->start();

    // Préparer et demarrer la publicité
    pAdvertising = pMyServer->getAdvertising();
    pAdvertising->start();

    Serial.println(F("BLE started"));
    Serial.println(F("En attente de connexion d'un client..."));
    // printSystemInfos();

    //------------Service : Battery Level-------------------
    /*
        // Create characteristic and descriptor
        BLECharacteristic BatterieLevelCharacteristic(BATTERIE_LEVEL_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
        BLEDescriptor BatterieLevelDescriptor(BATTERIE_LEVEL_DESCRIPTOR_UUID);
        BLEService *MyBatterieService = MyServer->createService(BATTERIE_SERVICE_UUID);

        MyBatterieService->addCharacteristic(&BatterieLevelCharacteristic);
        BatterieLevelCharacteristic.addDescriptor(&BatterieLevelDescriptor);

        uint8_t level = 17;
        BatterieLevelCharacteristic.setValue(&level, 1);
        BatterieLevelDescriptor.setValue("Niveau de charge en %");

        Serial.println("Lancement du service MyBatterieService");
        MyBatterieService->start();
    */
}

/// @brief Ajoute un descriptor à une caractéristique BLE
/// @param pCharacteristic Pointeur vers la caractéristique
/// @param uuid UUID du descriptor
/// @param valeur Valeur à assigner au descriptor
void addDescriptor(BLECharacteristic *pCharacteristic, const char *uuid, const char *valeur)
{
    BLEDescriptor *pDescriptor = new BLEDescriptor(uuid);
    pDescriptor->setValue(valeur);
    pCharacteristic->addDescriptor(pDescriptor);
}

void start_ble_server_HAL()
{
    // start_ble_service_HAL(pMyService);
    // start_ble_service_HAL(pServiceMessage);
    // start_ble_advertising_HAL(pAdvertising);
}

void stop_ble_server_HAL()
{
    // stop_ble_service_HAL(pMyService);
    // stop_ble_service_HAL(pService_UART);
    // stop_ble_advertising_HAL(pAdvertising);
}

// ------------------- Loop ---------------------------
/// @brief Boucle principale du serveur BLE à appeler dans loop()
void ble_server_loop_HAL()
{
    if (deviceConnected)
    {
        // --- every 1000 ms --------------------------------
        if (millis() - *pStartTimeNotify >= *pIntervalNotify)
        {
            *pStartTimeNotify = millis();

            // Exécuter les actions périodiques ici
            float tickRate = getTickRate();
            pCharacteristic_NOT_TICK_RATE->setValue(tickRate);
            pCharacteristic_NOT_TICK_RATE->notify();

            pCharacteristic_NOT_Test->setValue(&txValue, 1);
            pCharacteristic_NOT_Test->notify();
            txValue++;

            unsigned long time = millis();
            pRxCharacteristic_UART->setValue((uint8_t *)&time, sizeof(time));
            pRxCharacteristic_UART->notify();
        }
    }

    // On disconnecting
    if (!deviceConnected && oldDeviceConnected)
    {
        delay(500); // Attendre avant de redémarrer la publicité
        pMyServer->startAdvertising();
        Serial.println(F("En attente de connexion d'un client..."));
        oldDeviceConnected = deviceConnected;
    }

    // On connecting
    if (deviceConnected && !oldDeviceConnected)
    {
        oldDeviceConnected = deviceConnected;
    }
}

/// @brief Vérifie si un client BLE est connecté
/// @return true si un client est connecté
bool isDeviceConnected_HAL()
{
    return deviceConnected;
}

// ------------------- Callbacks Serveur BLE ---------------------------
/// @brief Callback lors de la connexion d'un client BLE
void MyServerCallbacks::onConnect(BLEServer *pMyServer)
{
    deviceConnected = true;
    // pMyServer->updatePeerMTU(pMyServer->getConnId(), 512); // Need to be negociated with client ?!
    Serial.println(F("BLE client Connecté !"));
    // printSystemInfos();
}
/// @brief Callback lors de la déconnexion d'un client BLE
void MyServerCallbacks::onDisconnect(BLEServer *pMyServer)
{
    deviceConnected = false;
    Serial.println(F("BLE client Déconnecté !"));
    // printSystemInfos();
}

/*
Principe d'echange client/serveur
NOTE: Le protocole BLE limite la taille des paquets à 20 Octets Maxi (20 caractères).
    Seule la requette READ implémente un systeme de transmission par paquets (chaque requette READ incremente le pointeur sur le paquet suivant).
    Le premier paquet contient le nombre de paquets totales afin d'en informer le client.

Imaginons une requette du client pour la capture infrarouge :
1- Le client envoie une requete WRITE 'IR_CAPTURE' sur la Charatéristic_COMMANDS
2- Le serveur execute la commande, le client peut (ou ne pas) s'attendre à une réponse du serveur.
3- Le serveur envoie une notification en réponse à l'opération sur la Cahracteristic_NOTIFY_COMMANDS :
      code 102 (Capture enregistrée). Le client peut traiter la réponse (par exemple executer une requette READ pour allez lire le resultat de la derniere capture.
      code 103 (Capture timeout). Le client peut eventuellement annuler son processus d'attente de réponse.
4- Si besoin, le client qui recoit une notification (en rapport avec la commande) peut ensuite executer une requette READ afin d'obtenir des détails sur les données correspndantes.

Canal de Notification :
      Les notifications en rapport avec les commandes envoient des codes sur la characteristic_COMMANDS.
      Les notifications autres envoient des messages sur la characteristic_NOTIFY

*/

// ------------------- Notify ---------------------------
/// @brief Envoie une notification BLE avec un code sur la caractéristique COMMANDS
/// @param code Code à envoyer
void sendBleNotifyCode_HAL(const std::string &code)
{
    sendBleNotify_HAL(pCharacteristic_NOTIFY_COMMANDS, code);
}

/// @brief Envoie une notification BLE pour une nouvelle commande capturée
/// @param command Commande capturée
void sendBleNotifyNewCommandCaptured_HAL(std::string command)
{
    sendBleNotify_HAL(pCharacteristic_NOTIFY_NEW_COMMAND_CAPTURED, command);
}

/// @brief Envoie une notification BLE générique
/// @param message Message à envoyer
void sendBleNotify_HAL(std::string message)
{
    sendBleNotify_HAL(pCharacteristic_Notify, message);
}

/// @brief Envoie une notification BLE sur une caractéristique spécifique
/// @param pCharacteristic Caractéristique BLE
/// @param message Message à envoyer
void sendBleNotify_HAL(BLECharacteristic *pCharacteristic, const std::string &message)
{
    if (deviceConnected)
    {
        // pCharacteristic->setValue((uint8_t*)&message, sizeof(message));
        // Serial.println("DEBUG2");
        pCharacteristic->setValue(message);
        pCharacteristic->notify();
        // Serial.println("DEBUG3");
        Serial.print(F("[NOTIFY] "));
        Serial.println(message.c_str());
    }
}
