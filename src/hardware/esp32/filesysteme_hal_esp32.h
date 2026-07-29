//#define SPIFFS_SYSTEM 1
//#define LITTLEFS_SYSTEM 2

/*
Les librairies SPIFFS et LitleFS partagent une API compatible mais ont des implémentations on-flash incompatibles,
il est donc important de choisir l'un ou l'autre par projet car tenter de monter un volume SPIFFS sous LittleFS peut
entraîner une opération de formatage et ne conservera certainement aucun fichier, et vice-versa. -versa.
*/

/*
#define FILE_SYSTEM LITTLEFS_SYSTEM

#if FILE_SYSTEM == SPIFFS_SYSTEM
  #include "hardware/esp32/filesystem_spiffs_hal_esp32.h"
#elif FILE_SYSTEM == LITTLEFS_SYSTEM
  #include "hardware/esp32/filesystem_littlefs_hal_esp32.h"
#else
  #error "FILE_SYSTEM not properly defined"
#endif
#include "interfaces/fileInfo.h"
*/

#define SPIFFS_SYSTEM 1
#define LITTLEFS_SYSTEM 2

#if defined(USE_SPIFFS)
    #define FILE_SYSTEM SPIFFS_SYSTEM
    #include "hardware/esp32/filesystem_spiffs_hal_esp32.h"
#elif defined(USE_LITTLEFS)
    #define FILE_SYSTEM LITTLEFS_SYSTEM
    #include "hardware/esp32/filesystem_littlefs_hal_esp32.h"
#else
    #error "FILE_SYSTEM not properly defined"
#endif
#include "interfaces/fileInfo.h"

// https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html

/*
SPIFFS (deprecated) :
Par convention pour la compatibilité avec cetaines bibliothèques, le nom des fichiers doit commencer par "/" (Non obligatoire)
Le nom des fichiers est limités à 31 caractères.
La structure est "à plat", il n'est pas possible de créer des répertoires. Le caractère "/" permet de simuler ces répertoires.
LittleFS :
LittleFS supporte la création de répertoires et est plus rapide que SPIFFS.
La longueur maximale des noms de fichiers est de 31 caractères, sans compter le chemin;
Les sous-répertoires sont créés et supprimés automatiquement selon les besoins.
*/

void test();
void fsFormat();
bool fsMount_HAL();
void fsUnMount_HAL();
void checkIfMounted_HAL();
void printFreeSpace();
unsigned int getUsedBytes_HAL();
unsigned int getTotalBytes_HAL();
String readFile_HAL(const String &path);
String readFile(fs::FS &fs, const String &path);
void writeFile_HAL(const String &path, const String &message);
void writeFile(fs::FS &fs, const String &path, const String &message);
void appendFile(const String &path, const String &message);
void appendFile(fs::FS &fs, const String &path, const String &message);
void renameFile(const String &path1, const String &path2);
void renameFile(fs::FS &fs, const String &path1, const String &path2);
void deleteFile_HAL(const String &path);
void deleteFile(fs::FS &fs, const String &path);

[[deprecated("Don't use this routine any more. Use the new one instead. 'listFilesInDirectory_HAL()'")]]
void listDir_HAL(const String &dirname, uint8_t levels);
[[deprecated("Don't use this routine any more. Use the new one instead. 'listFilesInDirectory()'")]]
void listDir(fs::FS &fs, const String &dirname, uint8_t levels);
void listFilesInDirectory_HAL(const String &dirname, uint8_t levels, FileCallback onFile);
void listFilesInDirectory(fs::FS &fs, const String &dirname, uint8_t levels, FileCallback onFile);

void testFileIO(const String &path);
void testFileIO(fs::FS &fs, const String &path);
