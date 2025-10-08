/// @file main.cpp
/// @brief Point d’entrée du firmware MaYaRemote pour ESP32.
/// @details Basé sur le projet [OMOTE](https://github.com/CoretechR/OMOTE/).
/// Ce fichier gère l’initialisation matérielle, les services système,
/// ainsi que la boucle principale exécutée en continu.
///
/// @author Christian LAMOTHE
/// @date 2023-2024

#include <Arduino.h>
#include "globals.h"
#include "PinDefinitionsAndMore.h"
#include "setup.h"
#include "system_info.h"
#include "tick_rate.h"
#include "sleep_manager.h"
#include "utils/digitalReadDelay.h"
#include "interfaces/hardwarePresenter.h"
#include "applicationInternal/commandHandler.h"
#include "applicationInternal/timeout.h"
#include "applicationInternal/command_ir_capture.h"
#include "applicationInternal/tasksManager2.h"

// SET_LOOP_TASK_STACK_SIZE( 16*1024 ); // 16KB

/// @brief Commande en cours d’exécution.
std::string command;
/// @brief Pointeur global vers le clignoteur de LED d’état.
LedBlinker *pGlobalStatusLED;
// unsigned char *ptrTest;

#if defined(ARDUINO)
// in case of Arduino we have a setup() and a loop()
/// @brief Fonction d’initialisation principale pour Arduino/ESP32.
/// comprends les fonctions Arduino setup() et loop()
/// @details Configure le port série, initialise le matériel,
/// affiche des infos système et lance les services de communication.
void setup()
{
#elif defined(WIN32) || defined(__linux__)
// in case of Windows/Linux, we have only a main() function, no setup() and loop(), so we have to simulate them
// forward declaration of loop()
void loop(unsigned long *pIMUTaskTimer, unsigned long *pUpdateStatusTimer);
// main function as usual in C
int main(int argc, char *argv[])
{
#endif
  /*
  #if ESP8266
    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  #else  // ESP8266
    Serial.begin(115200, SERIAL_8N1);
  #endif  // ESP8266
  */

  // --- Startup ---
  // USB.begin();
  Serial.begin(115200);
  // Serial.setRxBufferSize(1024);
  while (!Serial) // Wait for the serial connection to be establised.
    delay(50);
  Serial.println();
  Serial.println(F("Serial is ready"));

  SleepManager2().PrintWakeUpReason();
  Serial.println(F("------ System initialisation begin ------"));
  SleepManager2().init();
  printSystemInfo();

  pinMode(GPIO_CAPTURE_BUTTON, INPUT);

  pGlobalStatusLED = new LedBlinker(GPIO_STATUS_LED);
  pGlobalStatusLED->begin();

  // pinMode(STATUS_LED_PIN, OUTPUT);
  // digitalWrite(PIN_TEST_LED, HIGH);
  //  do some general hardware setup, like powering the TFT, I2C, ...
  //  init_hardware_general();
  //  get wakeup reason
  //  init_sleep();
  //  Restore settings from internal flash memory
  //  init_preferences();
  //  blinking led
  //  init_userled();
  //  Power Pin definition
  //  init_battery();

  // button Pin definition for hardware keys
  // init_keys();
  // setup IR sender
  init_infraredSender();
#if (ENABLE_KEYBOARD_BLE == 1)
  init_keyboardBLE();
#endif

  // ---- Fonctions en tests -------------

#if (ENABLED_BLE)
  init_ble_server();
  start_ble_server();
#endif

  set_irReceiverEnabled(true);
  // omote_log_d("will turn on IR receiver\r\n");
  start_infraredReceiver();

  init_commands();
  // register_device_test();
  //  ------------------------------------

  // register commands for the devices
  // register_specialCommands();
  //    TV
  // register_device_samsungTV();
  //  register_device_lgTV();
  //   AV receiver
  // register_device_yamahaAmp();
  // register_device_denonAvr();
  // register_device_lgsoundbar();
  //   media player
  // register_device_appleTV();
  // register_device_lgbluray();
  // register_device_samsungbluray();
  // register_device_shield();
  //   misc
  // register_device_smarthome();

#if (ENABLE_KEYBOARD_MQTT == 1)
  register_device_keyboard_mqtt();
#endif
#if (ENABLE_KEYBOARD_BLE == 1)
  register_device_keyboard_ble();
#endif
// register_keyboardCommands();

// Register the GUIs. They will be displayed in the order they have been registered.
// register_gui_sceneSelection();
// register_gui_irReceiver();
// register_gui_settings();
// register_gui_appleTV();
// register_gui_numpad();
// register_gui_smarthome();
// register_gui_yamahaAmp();
// Only show these GUIs in the main gui list. If you don't set this explicitely, by default all registered guis are shown.
#if (USE_SCENE_SPECIFIC_GUI_LIST != 0)
  main_gui_list = {tabName_yamahaAmp, tabName_sceneSelection, tabName_smarthome, tabName_settings, tabName_irReceiver};
#endif

// register the scenes and their key_commands_*
// register_scene_defaultKeys();
// register_scene_TV();
// register_scene_fireTV();
// register_scene_chromecast();
// register_scene_appleTV();
// register_scene_allOff();
// Only show these scenes on the sceneSelection gui. If you don't set this explicitely, by default all registered scenes are shown.
// set_scenes_on_sceneSelectionGUI({scene_name_TV, scene_name_fireTV, scene_name_chromecast, scene_name_appleTV});

// init GUI - will initialize tft, touch and lvgl
// init_gui();
// setLabelActiveScene();
// gui_loop(); // Run the LVGL UI once before the loop takes over

// setup the Inertial Measurement Unit (IMU) for motion detection. Has to be after init_gui(), otherwise I2C will not work
// init_IMU();

// init WiFi - needs to be after init_gui() because WifiLabel must be available
#if (ENABLE_WIFI_AND_MQTT == 1)
  init_mqtt();
#endif

  // omote_log_i("Setup finished in %lu ms.\r\n", millis());

#if defined(WIN32) || defined(__linux__)
  // In Windows/Linux there is no loop function that is automatically being called. So we have to do this on our own infinitely here in main()
  unsigned long IMUTaskTimer = 0;
  unsigned long updateStatusTimer = 0;
  while (1)
    loop(&IMUTaskTimer, &updateStatusTimer);
#endif
  Serial.println(F("------ MaYaRemote System Started ------"));
  // sendBleNotify("------ MaYaRemote System Started ------"); is not working because BLE is not started at this time.
  printSystemInfo();
}

// Loop ------------------------------------------------------------------------------------------------------------------------------------
#if defined(ARDUINO)
/// @brief Timer pour mise à jour IMU.
unsigned long IMUTaskTimer = 0;
/// @brief Timer pour mise à jour d’état du timer.
unsigned long updateStatusTimer = 0;
/// @brief Timer pour gestion du sommeil.
unsigned long updateSleepTimer = 0;
// unsigned long updateTickRateTimer = 0;

unsigned long *pIMUTaskTimer = &IMUTaskTimer;
unsigned long *pUpdateStatusTimer = &updateStatusTimer;
// unsigned long *pUpdateTickRateTimer = &updateTickRateTimer;
unsigned long *pUpdateSleepTimer = &updateSleepTimer;

/// @brief Callback appelé quand le bouton de capture IR est pressé (après délai).
void onButtonCaptureIRPressDelayed()
{
  Serial.println("Bouton de capture IR pressé (avec délai) !");
  executeCommand2("IR_CAPTURE", "");
}

/// @brief Boucle principale du firmware (Arduino).
/// @details Exécutée en continu, elle gère :
/// - la lecture du bouton capture IR,
/// - la mise à jour des tâches et timeouts,
/// - le traitement des commandes,
/// - la gestion du sommeil et des périphériques.
void loop()
{
  delay(10); // To reduced power consumption by increased main loop time execution
// yield();
#elif defined(WIN32) || defined(__linux__)
void loop(unsigned long *pIMUTaskTimer, unsigned long *pUpdateStatusTimer)
{
#endif

#if (ENABLED_BLE)
  ble_server_loop();
#endif

  // To execute IR Capture on button pressed and after delay.
  digitalReadDelay(GPIO_CAPTURE_BUTTON, CAPTURE_BUTTON_DELAY, onButtonCaptureIRPressDelayed);

  if (digitalRead(GPIO_CAPTURE_BUTTON) == LOW)
  {
    Serial.printf("Execute command at GPIO %u is pressed\r\n", GPIO_CAPTURE_BUTTON);
    // command = "LIST_COMMANDS";
    // command = "SAVE_COMMANDS";
    // command = "DELETE_COMMMANDS";
    // command = "IR_CAPTURE"; No need to execute Capture IR from this function, see digitalReadDelay instead.
    // command = "TEST_NOTIFY";
    // command = "IR_SONY_POWER";
    // command = "IR_MCE_POWER";
  }
  else
  {
    command = "";
  }

  if (command != "")
  {
    // We can use executeCommand rather than TaskManager because TaskManager is planned
    // to avoid stack overflow when task is executed from BLE and we don't use BLE here.
    executeCommand2(command, "");
  }

  // TasksManager::update();
  TasksManager2::update();

  // --- do as often as possible ----------------------------------------------
  // update backlight brightness. Fade in on startup, dim before going to sleep
  // update_backligthBrighness();
  // keypad handling: get key states from hardware and process them
  // keypad_loop();
  // process IR receiver, if activated
  if (get_irReceiverEnabled())
  {
    infraredReceiver_loop();
  }

  irCapture_loop();
  pGlobalStatusLED->update();
  TimeoutManager2::update();

  // --- every 1000 ms ----------------------------------
  // if (millis() - *pUpdateTickRateTimer >= 1000) {
  //*pUpdateTickRateTimer = millis();
  updateTickRate();
//}

// update LVGL UI
// gui_loop();
// call mqtt loop to receive mqtt messages, if you are subscribed to some topics
#if (ENABLE_WIFI_AND_MQTT == 1)
  mqtt_loop();
#endif

  // --- every 100 ms ------------------------------------
  // Refresh IMU data (motion detection) every 100 ms
  // If no action (key, TFT or motion), then go to sleep
  if (millis() - *pIMUTaskTimer >= 100)
  {
    *pIMUTaskTimer = millis();
    // check_activity();
  }

  // --- every 1000 ms -----------------------------------
  if (millis() - *pUpdateStatusTimer >= 1000)
  {
    *pUpdateStatusTimer = millis();
    // update user_led, battery, BLE, memoryUsage on GUI
    // updateHardwareStatusAndShowOnGUI();
  }

  // Affichage de l'état de la mémoire toutes les 5 secondes
  static unsigned long lastMemCheck = 0;
  if (millis() - lastMemCheck > 5000)
  {
    lastMemCheck = millis();
    // Serial.println("Live memory usage: ");
    // printSystemInfo();
  }

  // --- every 10 s --------------------------------------
  if (millis() - *pUpdateSleepTimer >= 10000)
  {
    *pUpdateSleepTimer = millis();
    SleepManager2().update();
  }
}