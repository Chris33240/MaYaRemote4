#include "filesystem_littlefs_hal_esp32.h"
#include "helpers/omote_log.h"

namespace nsLittleFS
{
    /// @brief Formate le système de fichiers LittleFS.
    /// @note Ne pas utiliser en production !
    /// @warning Cette fonction est commentée pour éviter la perte de données.
    void format()
    {
        // DON'T USE FORMAT PLEASE !!!
        // if (!Little.format()) {
        // Serial.println("An Error has occurred while formating Little");
        // throw std::runtime_error("An error has occurred while formating Little");
        //}
    }

    /// @brief Monte le système de fichiers LittleFS.
    /// @return true si le montage réussit.
    /// @throws std::runtime_error si le montage échoue.
    bool mount()
    {
        bool isMounted;
#ifdef FORMAT_FILE_SYSTEM_IF_FAILED
    //omote_log_v("DEBUG: we are here 7\r\n");
    //omote_log_v_mem();
        isMounted = LittleFS.begin(FORMAT_FILE_SYSTEM_IF_FAILED);
    //omote_log_v("DEBUG: we are here 8\r\n");
    //omote_log_v_mem();
#endif
        if (!isMounted)
        {
            Serial.println("An Error has occurred while mounting LittleFS");
            throw std::runtime_error("An error has occurred while mounting LittleFS");
        }
        return isMounted;
    }

    /// @brief Démonte le système de fichiers LittleFS et libère les ressources.
    void unMount()
    {
        LittleFS.end();
    }

    /// @brief Affiche des informations sur l’espace total et utilisé du FS LittleFS.
    void printFreeSpace()
    {
        unsigned int totalBytes = LittleFS.totalBytes();
        unsigned int usedBytes = LittleFS.usedBytes();
        Serial.println("========== File System Info ===========");
        Serial.println("totalBytes : " + totalBytes);
        Serial.println("usedBytes : " + usedBytes);
    }

    /// @brief Récupère la quantité d’octets utilisés dans LittleFS.
    /// @return Nombre d’octets utilisés.
    unsigned int getUsedBytes()
    {
        return LittleFS.usedBytes();
    }

    /// @brief Récupère la capacité totale du FS LittleFS.
    /// @return Nombre total d’octets disponibles.
    unsigned int getTotalBytes()
    {
        return LittleFS.totalBytes();
    }

}