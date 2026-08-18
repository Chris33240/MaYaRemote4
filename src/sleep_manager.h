#pragma once

#include <Arduino.h>
#include <driver/rtc_io.h>
#include <esp_sleep.h>
#include <soc/rtc.h>
#include <soc/rtc_cntl_reg.h>
#include <soc/soc.h>
#include "time_manager.h"

#define GLOBAL_TIMER "global"
#define GLOBAL_TIME (180UL * 60UL * 1000UL)  // 180 minutes en ms
#define DISCONNECT_TIMER "disconnect"
#define DISCONNECT_TIME (30UL * 60UL * 1000UL) // 30 minutes en ms

// Valeurs test :
// #define DISCONNECT_TIME (30 * 1000)  // 30 secondes en ms
// #define GLOBAL_TIME (60 * 1000)    // 60 secondes en ms

#define RTC_CNTL_RTC_WAKEUP_ENA_S 0                      // Décalage du bit
#define RTC_CNTL_RTC_WAKEUP_ENA_M ((uint32_t)0xFFFFFFFF) // Masque pour les bits

// Définition du décalage et du masque pour le bit 12
#define RTC_CNTL_RTC_WAKEUP_ENA_BIT 12
#define RTC_CNTL_RTC_WAKEUP_ENA_MASK (1 << RTC_CNTL_RTC_WAKEUP_ENA_BIT)
// Adresse du registre RTC_CNTL_RTC_WAKEUP_ENA
// #define RTC_CNTL_RTC_WAKEUP_ENA_REG (0x60000700 + 0x20)  // Adresse fictive, à ajuster selon votre documentation
#define RTC_CNTL_RTC_WAKEUP_STATE_REG (0x60008000 + 0x003C)
#define RTC_CNTL_EXT_WAKEUP0_LV_REG ()

// page 404 :
// Low-Power Management : 0x6000 8000  à 0x6000 8FFF

// Adresse du regitre RTC_CNTL_RTC_EXT_WAKEUP_CONF_REG
#define RTC_CNTL_RTC_EXT_WAKEUP_CONF_REG (0x60008000 + 0x0064)
// Définition du decalage pour le bit 30
#define RTC_CNTL_EXT_WAKEUP0_LV_BIT 30

// Adresse du regitre RTC_CNTL_DIG_PWC_REG
// #define RTC_CNTL_DIG_PWC_REG (0x60008000 + 0x0090)
// Définition du décalage et pour le bit 30
// #define RTC_CNTL_WIFI_PD_EN 30

// #define REG_SET_BIT(reg, bit) (*((volatile uint32_t *)(reg)) |= (bit))
// #define REG_CLR_BIT(reg, bit) (*((volatile uint32_t *)(reg)) &= ~(bit))
// #define REG_WRITE(reg, val) (*((volatile uint32_t *)(reg)) = (val))
// #define REG_READ(reg) (*((volatile uint32_t *)(reg)))

class SleepManager
{
public:
    SleepManager();

    static void init();
    static void update();

    static void pause();
    static void resume();

    static void onConnected();
    static void onDisconnected();

    static unsigned long getSleepTimerRemaining();
    static uint32_t getSleepTimerRemainingSeconds();

    static void PrintWakeUpReason();

private:
    //static bool isConnected;

    static void onDisconnectTimerElapsed();
    static void onGlobalTimerElapsed();

    static void goToSleep();
    static void esp_sleep_enable_bt_wakeup();

    static void set_bit_in_register(uint32_t reg_addr, uint32_t bit_mask, bool value);
};