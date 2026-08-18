#include "sleep_manager.h"
#include "PinDefinitionsAndMore.h"
#include "interfaces/hardwarePresenter.h"

/// @brief Constructeur de la classe SleepManager2.
/// Initialise un gestionnaire de mise en veille.
SleepManager::SleepManager()
{
}

/// @brief Initialise le gestionnaire de veille.
/// Configure le bouton de réveil GPIO et initialise les registres.
/// Active les sources de réveil par GPIO et affiche les registres pour debug.
void SleepManager::init()
{
    // La pin GPIO_WAKEUP_BUTTON est configurée comme GPIO_RTC dans goToSleep() et n'est pas utilisé en dehors de cette fonction,
    // il n'est donc pas obligatoire de configurer comme GPIO standard ici.
    // A noter aussi que certaines cartes peuvent déjà avoir une resistance de tirage externe de 10kOhms (relièe en pull-up).
    pinMode(GPIO_WAKEUP_BUTTON, INPUT_PULLUP); // Initialisation de la pin GPIO en entrée

    //rtc_gpio_init((gpio_num_t)GPIO_WAKEUP_BUTTON);
    //rtc_gpio_set_direction((gpio_num_t)GPIO_WAKEUP_BUTTON, RTC_GPIO_MODE_INPUT_ONLY);
    //rtc_gpio_pullup_en((gpio_num_t)GPIO_WAKEUP_BUTTON);
    //rtc_gpio_pulldown_dis((gpio_num_t)GPIO_WAKEUP_BUTTON);

    // resetTimers();
    //  Configurer et initialiser Bluetooth
    // esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    // esp_bt_controller_init(&bt_cfg);
    // esp_bt_controller_enable(ESP_BT_MODE_BLE);
    // esp_bluedroid_init();
    // esp_bluedroid_enable();

    // esp_bt_controller_mem_release(ESP_BT_MODE_BLE);
    // esp_bt_controller_enable(ESP_BT_MODE_BLE);
    // esp_bt_sleep_enable();
    // esp_bt_sleep_disable();
    // esp_bt_controller_wakeup_request();
    // esp_sleep_enable_bt_wakeup(); // Réveil par Bluetooth (optionnel)

    // enableBluetoothWakeup();

    // Configurer les interruptions RTC pour réveil Bluetooth
    // Activer l'interruption Bluetooth pour le réveil
    // REG_SET_BIT(RTC_CNTL_INT_ENA_REG, RTC_CNTL_BT_WAKEUP_INT_ENA);

    // Nettoyer les interruptions précédentes
    // REG_SET_BIT(RTC_CNTL_INT_CLR_REG, RTC_CNTL_BT_WAKEUP_INT_CLR);

    // Configurer l'état RTC pour le mode veille avec Bluetooth actif
    // REG_WRITE(RTC_CNTL_STATE0_REG, RTC_CNTL_BT_ACTIVE | RTC_CNTL_SLEEP_EN);

    // Exemple d'écriture dans le registre
    // uint32_t value_to_write = 0x1;  // Valeur à écrire, ajustez selon vos besoins
    //*((volatile uint32_t *)RTC_CNTL_RTC_WAKEUP_ENA_REG) = (value_to_write & RTC_CNTL_RTC_WAKEUP_ENA_M);

    // Exemple d'écriture dans le bit 12 du registre
    // uint32_t value_to_write = 0x1;  // Mettre à 1 pour activer
    //*((volatile uint32_t *)RTC_CNTL_RTC_WAKEUP_STATE_REG) |= (value_to_write << RTC_CNTL_RTC_WAKEUP_ENA_BIT);

    // uint32_t defaultValue = *((volatile uint32_t *)RTC_CNTL_RTC_WAKEUP_STATE_REG);
    // Serial.println(defaultValue, HEX); // Affiche la valeur en hexadécimal

    uint32_t regValue;

    regValue = *((volatile uint32_t *)RTC_CNTL_DIG_PWC_REG);
    Serial.printf("Valeur initiale du registre 0x%08X: 0x%08X\r\n", RTC_CNTL_DIG_PWC_REG, regValue);
    // set_bit_in_register(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_PD_EN, true);

    // Registre: sources de reveil
    regValue = *((volatile uint32_t *)RTC_CNTL_RTC_WAKEUP_STATE_REG);
    Serial.printf("Valeur initiale du registre 0x%08X: 0x%08X\r\n", RTC_CNTL_RTC_WAKEUP_STATE_REG, regValue);

    // Fonction du framework Arduino
    //esp_sleep_enable_ext0_wakeup((gpio_num_t)GPIO_WAKEUP_BUTTON, 0); // Réveil par GPIO

    // Fonction réecrite
    // esp_sleep_enable_ext0_wakeup1((gpio_num_t)GPIO_WAKEUP_PIN, 0); // Réveil par GPIO
    regValue = *((volatile uint32_t *)RTC_CNTL_RTC_WAKEUP_STATE_REG);
    Serial.printf("Valeur initiale du registre 0x%08X aprés init ext0: 0x%08X\r\n", RTC_CNTL_RTC_WAKEUP_STATE_REG, regValue);

    // esp_sleep_enable_bt_wakeup(); // Réveil par BT (Does not work)
    regValue = *((volatile uint32_t *)RTC_CNTL_RTC_WAKEUP_STATE_REG);
    Serial.printf("Valeur initiale du registre 0x%08X après modification: 0x%08X\r\n", RTC_CNTL_RTC_WAKEUP_STATE_REG, regValue);

    // Désactive les priphériques RTC pour passer en hibernation avec la commande : esp_deep_sleep_start();
    // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    // esp_sleep_pd_config(ESP_PD_DOMAIN_CPU, ESP_PD_OPTION_OFF);

    // Wifi ou Bluetooth is not working under 80MHz
    // setCpuFrequencyMhz(20);
    // setCpuFrequencyMhz(40);
    // setCpuFrequencyMhz(80);

    // esp_sleep_enable_wifi_wakeup();

    /*
    // Configure Bluetooth modem sleep with higher minimum frequency
    esp_pm_config_esp32_t pm_config = {
        .max_freq_mhz = 80,
        .min_freq_mhz = 40,
    #if CONFIG_FREERTOS_USE_TICKLESS_IDLE
        .light_sleep_enable = true
    #endif
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
    */

    /*
    // Configurer les fréquences minimales et maximales
    // Wifi ou Bluetooth is not working under 80MHz
    esp_pm_config_esp32_t pm_config = {
        .max_freq_mhz = 240, // Fréquence maximale en MHz
        .min_freq_mhz = 80,  // Fréquence minimale en MHz
        .light_sleep_enable = true // Permettre le mode light sleep
    };

    // Appliquer la configuration
    esp_err_t result = esp_pm_configure(&pm_config);
    if (result == ESP_OK) {
        Serial.println("Configuration de la fréquence dynamique réussie.");
    } else {
        Serial.print("Erreur lors de la configuration de la fréquence dynamique: ");
        Serial.println(esp_err_to_name(result));
    }
    */

    TimeManager::init();

    TimeManager::addTimer(
        DISCONNECT_TIMER,                               // Timer key for timers
        {
            DISCONNECT_TIME,                            // Duration
            0,                                          // Elapsed
            false,                                      // Pause
            SleepManager::onDisconnectTimerElapsed     // OnElapsed (Callback)
        });

    TimeManager::addTimer(
        GLOBAL_TIMER,
        {
            GLOBAL_TIME,
            0,
            false,
            SleepManager::onGlobalTimerElapsed
        });
}

/// @brief Met à jour les timers de mise en veille.
/// Calcule le delta temps, incrémente les compteurs et déclenche les actions
/// (mise en veille) si les délais sont dépassés.
void SleepManager::update()
{
    TimeManager::update();
    if(isDeviceConnected()) {
        onConnected();  
    } else {
        onDisconnected();
    }
}

/// @brief Met le gestionnaire en pause.
/// Suspend temporairement la gestion des timers d’activité et de déconnexion.
void SleepManager::pause()
{
    TimeManager::pause();
}

/// @brief Reprend la gestion des timers après une pause.
void SleepManager::resume()
{
    TimeManager::resume();
}

void SleepManager::onConnected()
{
    TimeManager::resetTimer(DISCONNECT_TIMER);
    TimeManager::pauseTimer(DISCONNECT_TIMER);
}

void SleepManager::onDisconnected()
{
    TimeManager::resumeTimer(DISCONNECT_TIMER);
}

void SleepManager::onDisconnectTimerElapsed()
{
    goToSleep();
}

void SleepManager::onGlobalTimerElapsed()
{
    goToSleep();
}

unsigned long SleepManager::getSleepTimerRemaining()
{
    return TimeManager::getMinimumRemaining();
}

uint32_t SleepManager::getSleepTimerRemainingSeconds()
{
    return static_cast<uint32_t>(TimeManager::getMinimumRemaining() / 1000);
}

//==============================================================
// Gestion du registre matériel
//==============================================================

/// @brief Modifie un bit dans un registre matériel.
/// @param reg_addr Adresse du registre.
/// @param bit_mask Masque du bit à modifier.
/// @param value true pour activer le bit, false pour le désactiver.
void SleepManager::set_bit_in_register(uint32_t reg_addr, uint32_t bit_mask, bool value)
{
    // Lire la valeur actuelle du registre
    uint32_t reg_value = *((volatile uint32_t *)reg_addr);

    if (value)
    {
        // Mettre le bit à 1 (activer)
        reg_value |= bit_mask;
    }
    else
    {
        // Mettre le bit à 0 (désactiver)
        reg_value &= ~bit_mask;
    }

    // Écrire la nouvelle valeur dans le registre
    *((volatile uint32_t *)reg_addr) = reg_value;

    // Affichage pour debug
    // ESP_LOGI("set_bit_in_register", "Registre 0x%08X modifié à 0x%08X", reg_addr, reg_value);
    Serial.printf("[set_bit_in_register] Registre 0x%08X modifié à 0x%08X\r\n", reg_addr, reg_value);
}

/*
void SleepManager2::esp_sleep_enable_ext0_wakeup1(uint32_t gpio_pin, uint32_t level) {
    // Lire la valeur actuelle du registre
    uint32_t regValue = *((volatile uint32_t *)RTC_CNTL_RTC_EXT_WAKEUP_CONF_REG);

    // Configurer le pin RTC pour EXT0 wakeup
    *((volatile uint32_t *)RTC_IO_EXT_WAKEUP0_SEL) = gpio_pin;

    // Modifier le bit 30 en fonction du niveau
    if (level == HIGH) {
        regValue |= (1U << RTC_CNTL_EXT_WAKEUP0_LV_BIT); // Définir le bit 30 à 1
    } else {
        regValue &= ~(1U << RTC_CNTL_EXT_WAKEUP0_LV_BIT); // Réinitialiser le bit 30 à 0
    }

    // Écrire la nouvelle valeur dans le registre
    *((volatile uint32_t *)RTC_CNTL_RTC_EXT_WAKEUP_CONF_REG) = regValue;

    Serial.printf("Configuration EXT0 : GPIO=%u, Niveau=%u, Registre=0x%08X\n", gpio_pin, level, regValue);
}*/

//==============================================================
// Activation du réveil Bluetooth
//==============================================================

/// @brief Active la source de réveil Bluetooth via manipulation des registres RTC.
void SleepManager::esp_sleep_enable_bt_wakeup()
{
    uint32_t regValue = *((volatile uint32_t *)RTC_CNTL_RTC_WAKEUP_STATE_REG);

    // Calcul du masque pour cibler les bits de la position 15 à 31
    uint32_t mask = ((1U << (31 - 15 + 1)) - 1) << 15;

    // Nettoyer les bits ciblés
    regValue &= ~mask;

    // Insérer la valeur 0x400 aux bits ciblés
    regValue |= (0x400 << 15) & mask;

    // Écrire dans le registre
    *((volatile uint32_t *)RTC_CNTL_RTC_WAKEUP_STATE_REG) = regValue;
    // Serial.printf("Valeur du registre à ecrire: 0x%08X\n", regValue);
}

//==============================================================
// Mise en veille
//==============================================================

/// @brief Met l’ESP32 en veille profonde (deep sleep).
/// Désactive certains périphériques et éteint les GPIOs avant d’entrer en mode sommeil.
void SleepManager::goToSleep()
{
    Serial.println("Entering deep sleep...");
    Serial.flush();
    // esp_sleep_enable_ext0_wakeup(GPIO_WAKEUP_PIN, 0); // Réveil par GPIO
    // esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(GPIO_WAKEUP_PIN), 0); // Réveil par GPIO
    ////////esp_sleep_enable_ext0_wakeup((gpio_num_t)GPIO_WAKEUP_PIN, 0); // Réveil par GPIO
    // esp_sleep_enable_timer_wakeup(1000000 * 10);  // Réveil après 10 secondes en complément
    // esp_sleep_enable_bt_wakeup(); // Réveil par Bluetooth (optionnel)
    // digitalWrite(GPIO_BUILT_IN_LED_IO2, LOW); // Turn off Built-in LED "ON"
    digitalWrite(GPIO_POWER, LOW); // Turn off IR receiver
    // esp_light_sleep_start(); // Démarrer la mise en veille légère
    // esp_sleep_enable_timer_wakeup(30 * 1000 * 1000);
    // esp_wifi_bt_power_domain_off();
    // esp_bluedroid_disable();
    // esp_bt_controller_disable();
    // delay(100);

/*
rtc_gpio_deinit((gpio_num_t)GPIO_WAKEUP_BUTTON);
pinMode(GPIO_WAKEUP_BUTTON, INPUT_PULLUP);

esp_sleep_enable_ext0_wakeup((gpio_num_t)GPIO_WAKEUP_BUTTON, 0); // Réveil par GPIO

Serial.print("GPIO juste avant sommeil = ");
Serial.println(digitalRead(GPIO_WAKEUP_BUTTON));
delay(3000);

    esp_deep_sleep_start(); // Démarrer la mise en veille profonde
*/


/*
rtc_gpio_deinit((gpio_num_t)GPIO_WAKEUP_BUTTON);
pinMode(GPIO_WAKEUP_BUTTON, INPUT_PULLUP);

ESP_ERROR_CHECK(
    esp_sleep_enable_ext0_wakeup((gpio_num_t)GPIO_WAKEUP_BUTTON, 0) // Réveil par GPIO
);

Serial.print("GPIO juste avant sommeil = ");
Serial.println(digitalRead(GPIO_WAKEUP_BUTTON));
delay(3000);

    esp_deep_sleep_start(); // Démarrer la mise en veille profonde
*/

// --------------------------------------------------------------
// Reconfiguration pin et wakup avant l'entrée en veille profonde
// --------------------------------------------------------------
//Pour une même Pin, il y a 2 etats de configurations spcéifiques aux GPIO standard et GPIO RTC (utilisé pour EXT0)
//La configuration par pinMode() effectuées précédement (GPIO standard) peut aussi modifier l'etat de cette configuration pour GPIO RTC.
//Si cela arrive, il faut réinitialiser cet etat avec rtc_gpio_deinit() avant de réutiliser le GPIO RTC comme source de réveil.
rtc_gpio_deinit((gpio_num_t)GPIO_WAKEUP_BUTTON);
rtc_gpio_init((gpio_num_t)GPIO_WAKEUP_BUTTON);
rtc_gpio_set_direction((gpio_num_t)GPIO_WAKEUP_BUTTON, RTC_GPIO_MODE_INPUT_ONLY);
rtc_gpio_pullup_en((gpio_num_t)GPIO_WAKEUP_BUTTON);
rtc_gpio_pulldown_dis((gpio_num_t)GPIO_WAKEUP_BUTTON);

//Code recommandé en RELEASE
ESP_ERROR_CHECK(
    esp_sleep_enable_ext0_wakeup((gpio_num_t)GPIO_WAKEUP_BUTTON, 0)
);
//Code recommandé en DEBUG
//esp_err_t err = esp_sleep_enable_ext0_wakeup((gpio_num_t)GPIO_WAKEUP_BUTTON, 0); // Réveil par GPIO
//Serial.printf("ext0 = %s\n", esp_err_to_name(err));

Serial.print("GPIO juste avant sommeil = ");
Serial.println(digitalRead(GPIO_WAKEUP_BUTTON));
delay(3000);

    esp_deep_sleep_start(); // Démarrer la mise en veille profonde
}

//==============================================================
// Cause du réveil
//==============================================================

/// @brief Affiche la cause du réveil après une sortie de veille.
/// Les causes possibles incluent EXT0, EXT1, TIMER, TOUCHPAD, ULP, GPIO, UART, etc.
void SleepManager::PrintWakeUpReason()
{
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason)
    {
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("Wakeup caused by EXT0");
            break;
        case ESP_SLEEP_WAKEUP_EXT1:
            Serial.println("Wakeup caused by EXT1");
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("Wakeup caused by TIMER");
            break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            Serial.println("Wakeup caused by TOUCHPAD");
            break;
        case ESP_SLEEP_WAKEUP_ULP:
            Serial.println("Wakeup caused by ULP");
            break;
        case ESP_SLEEP_WAKEUP_GPIO:
            Serial.println("Wakeup caused by GPIO");
            break;
        case ESP_SLEEP_WAKEUP_UART:
            Serial.println("Wakeup caused by UART");
            break;
        default:
            Serial.println("Wakeup without reason");
            break;
    }
}
