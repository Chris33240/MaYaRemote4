#include "filesysteme_hal_esp32.h"

// https://www.tutorialspoint.com/esp32_for_iot/esp32_for_iot_spiffs_storage.htm

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */

/// @brief Indique si le système de fichiers est monté.
bool isMounted;
// using fs1 = SPIFFS;
// fs::FS fs1 = SPIFFS;
// fs::FS& fs1;

#if defined(FILE_SYSTEM) && FILE_SYSTEM == SPIFFS_SYSTEM
fs::FS fs1 = SPIFFS;
#elif defined(FILE_SYSTEM) && FILE_SYSTEM == LITTLEFS_SYSTEM
fs::FS fs1 = LittleFS;
#endif

/// @brief Exécute une série de tests sur le système de fichiers.
/// @details Monte le FS, écrit, lit, renomme et supprime plusieurs fichiers
/// pour valider le bon fonctionnement de l’implémentation.
/// @throws std::runtime_error si une opération échoue.
void test()
{

   try
   {
      fsMount_HAL();
      writeFile_HAL("/hello.txt", "Hello ");
      // writeFile(SPIFFS, "/hello.txt", "Hello ");
      appendFile("/hello.txt", "World!\r\n");
      // appendFile(SPIFFS, "/hello.txt", "World!\r\n");
      readFile_HAL("/hello.txt");
      // readFile(SPIFFS, "/hello.txt");
      renameFile("/hello.txt", "/foo.txt");
      // renameFile(SPIFFS, "/hello.txt", "/foo.txt");
      readFile_HAL("/foo.txt");
      // readFile(SPIFFS, "/foo.txt");
      deleteFile_HAL("/foo.txt");
      // deleteFile(SPIFFS, "/foo.txt");
      testFileIO("/test.txt");
      // testFileIO(SPIFFS, "/test.txt");
      deleteFile_HAL("/test.txt");
      // deleteFile(SPIFFS, "/test.txt");
      fsUnMount_HAL();
      Serial.println("Test complete");
   }
   catch (const std::runtime_error &e)
   {
      Serial.println(e.what());
   }
}

/// @brief Formate le système de fichiers.
/// @note Efface toutes les données.
void fsFormat()
{
#if defined(FILE_SYSTEM) && FILE_SYSTEM == SPIFFS_SYSTEM
   nsSPIFFS::Format();
#elif defined(FILE_SYSTEM) && FILE_SYSTEM == LITTLEFS_SYSTEM
   nsLittleFS::format();
#else
#error "No filesystem selected"
#endif
}

/// @brief Monte le système de fichiers.
/// @return true si le montage réussit, false sinon.
bool fsMount_HAL()
{
#if defined(FILE_SYSTEM) && FILE_SYSTEM == SPIFFS_SYSTEM
   isMounted = nsSPIFFS::Mount();
#elif defined(FILE_SYSTEM) && FILE_SYSTEM == LITTLEFS_SYSTEM
   isMounted = nsLittleFS::mount();
#else
#error "No filesystem selected"
#endif
   return isMounted;
}

/// @brief Démonte le système de fichiers et libère les ressources.
void fsUnMount_HAL()
{
#if defined(FILE_SYSTEM) && FILE_SYSTEM == SPIFFS_SYSTEM
   nsSPIFFS::UnMount();
#elif defined(FILE_SYSTEM) && FILE_SYSTEM == LITTLEFS_SYSTEM
   nsLittleFS::unMount();
#else
#error "No filesystem selected"
#endif
   isMounted = false;
}

/// @brief Vérifie que le FS est monté.
/// @throws std::runtime_error si le FS n’est pas monté.
void checkIfMounted_HAL()
{
   if (!isMounted)
   {
      throw std::runtime_error("File System must be mounted before using it");
   }
}

/// @brief Affiche l’espace libre disponible dans le FS.
void printFreeSpace()
{
   checkIfMounted_HAL();
#if defined(FILE_SYSTEM) && FILE_SYSTEM == SPIFFS_SYSTEM
   nsSPIFFS::PrintFreeSpace();
#elif defined(FILE_SYSTEM) && FILE_SYSTEM == LITTLEFS_SYSTEM
   nsLittleFS::printFreeSpace();
#else
#error "No filesystem selected"
#endif
}

/// @brief Retourne l’espace utilisé dans le FS.
/// @return Nombre d’octets utilisés.
unsigned int getUsedBytes_HAL()
{
   checkIfMounted_HAL();
#if defined(FILE_SYSTEM) && FILE_SYSTEM == SPIFFS_SYSTEM
   return nsSPIFFS::getUsedBytes();
#elif defined(FILE_SYSTEM) && FILE_SYSTEM == LITTLEFS_SYSTEM
   return nsLittleFS::getUsedBytes();
#else
#error "No filesystem selected"
#endif
}

/// @brief Retourne la taille totale du FS.
/// @return Nombre total d’octets disponibles.
unsigned int getTotalBytes_HAL()
{
   checkIfMounted_HAL();
#if defined(FILE_SYSTEM) && FILE_SYSTEM == SPIFFS_SYSTEM
   return nsSPIFFS::getTotalBytes();
#elif defined(FILE_SYSTEM) && FILE_SYSTEM == LITTLEFS_SYSTEM
   return nsLittleFS::getTotalBytes();
#else
#error "No filesystem selected"
#endif
}

/// @brief Affiche et retourne le contenu d’un fichier.
/// @param path Chemin du fichier.
/// @return Contenu du fichier sous forme de chaîne.
String readFile_HAL(const String &path)
{
   return readFile(fs1, path);
}

/// @brief Affiche et retourne le contenu d’un fichier.
/// @param fs Instance du FS.
/// @param path Chemin du fichier.
/// @return Contenu du fichier.
String readFile(fs::FS &fs, const String &path)
{
   checkIfMounted_HAL();
   Serial.printf("Reading file: %s\r\n", path.c_str());

   File file = fs.open(path);
   if (!file || file.isDirectory())
   {
      throw std::runtime_error(("Reading: failed to open file for reading: " + path).c_str());
   }

   String text = file.readString();
   file.close();

   return text;
}

/// @brief Écrit dans un fichier (remplace le contenu).
/// @param path Chemin du fichier.
/// @param message Contenu à écrire.
void writeFile_HAL(const String &path, const String &message)
{
   writeFile(fs1, path, message);
}

/// @brief Écrit dans un fichier (remplace le contenu).
/// @param fs Instance du FS.
/// @param path Chemin du fichier.
/// @param message Contenu à écrire.
void writeFile(fs::FS &fs, const String &path, const String &message)
{
   checkIfMounted_HAL();
   Serial.printf("Writing file: %s\r\n", path.c_str());

   File file = fs.open(path, FILE_WRITE);
   if (!file)
   {
      throw std::runtime_error(("Writing: failed to open file for writing: " + path).c_str());
   }
   if (file.print(message))
   {
      Serial.println("Writing: file written");
   }
   else
   {
      throw std::runtime_error(("Writing: file failed: " + path).c_str());
   }
   file.close();
}

/// @brief Ajoute du contenu à la fin d’un fichier.
/// @param path Chemin du fichier.
/// @param message Contenu à ajouter.
void appendFile(const String &path, const String &message)
{
   appendFile(fs1, path, message);
}

/// @brief Ajoute du contenu à la fin d’un fichier.
/// @param fs Instance du FS.
/// @param path Chemin du fichier.
/// @param message Contenu à ajouter.
void appendFile(fs::FS &fs, const String &path, const String &message)
{
   checkIfMounted_HAL();
   Serial.printf("Appending to file: %s\r\n", path.c_str());

   File file = fs.open(path, FILE_APPEND);
   if (!file)
   {
      throw std::runtime_error(("Appending: failed to open file for appending: " + path).c_str());
   }
   if (file.print(message))
   {
      Serial.println("Appending: message appended");
   }
   else
   {
      throw std::runtime_error(("Appending: append failed: " + path).c_str());
   }
   file.close();
}

/// @brief Renomme un fichier.
/// @param path1 Chemin source.
/// @param path2 Chemin destination.
void renameFile(const String &path1, const String &path2)
{
   renameFile(fs1, path1, path2);
}

/// @brief Renomme un fichier.
/// @param fs Instance du FS.
/// @param path1 Chemin source.
/// @param path2 Chemin destination.
void renameFile(fs::FS &fs, const String &path1, const String &path2)
{
   checkIfMounted_HAL();
   Serial.printf("Renaming file %s to %s\r\n", path1.c_str(), path2.c_str());
   if (fs.rename(path1, path2))
   {
      Serial.println("Rename: file renamed");
   }
   else
   {
      throw std::runtime_error(("Rename: rename failed: " + path1).c_str());
   }
}

/// @brief Supprime un fichier.
/// @param path Chemin du fichier.
void deleteFile_HAL(const String &path)
{
   deleteFile(fs1, path);
}

/// @brief Supprime un fichier.
/// @param fs Instance du FS.
/// @param path Chemin du fichier.
void deleteFile(fs::FS &fs, const String &path)
{
   checkIfMounted_HAL();
   Serial.printf("Deleting file: %s\r\n", path.c_str());
   if (fs.remove(path))
   {
      Serial.println("Delete: file deleted");
   }
   else
   {
      throw std::runtime_error(("Delete: delete failed: " + path).c_str());
   }
}

/// @brief Affiche le contenu d’un répertoire et sous-répertoires.
/// @param dirname Nom du répertoire.
/// @param levels Profondeur de récursion.
void listDir_HAL(const String &dirname, uint8_t levels)
{
   listDir(fs1, dirname, levels);
}

/// @brief Affiche le contenu d’un répertoire et sous-répertoires.
/// @param fs Instance du FS.
/// @param dirname Nom du répertoire.
/// @param levels Profondeur de récursion.
void listDir(fs::FS &fs, const String &dirname, uint8_t levels)
{
   checkIfMounted_HAL();
   Serial.printf("Listing directory: %s\r\n", dirname);

   File root = fs.open(dirname);
   if (!root)
   {
      throw std::runtime_error(("ListDir: failed to open directory: " + dirname).c_str());
   }
   if (!root.isDirectory())
   {
      root.close();
      throw std::runtime_error(("ListDir: not a directory: " + dirname).c_str());
   }

   File file = root.openNextFile();
   while (file)
   {
      if (file.isDirectory())
      {
         Serial.print("  DIR : ");
         Serial.println(file.name());
         if (levels)
         {
            listDir(fs, file.name(), levels - 1);
         }
      }
      else
      {
         Serial.print("  FILE: ");
         Serial.print(file.name());
         Serial.print("\tSIZE: ");
         Serial.println(file.size());
      }
      file.close();
      file = root.openNextFile();
   }
   root.close();
}

/// @brief Affiche le contenu d’un répertoire et sous-répertoires.
/// @param dirname Nom du répertoire.
/// @param levels Profondeur de récursion.
/// @param onFile Callback de lecture de fichier.
/// @param onError Callback d'erreur.
void listFilesInDirectory_HAL(const String &dirname, uint8_t levels, FileCallback onFile)
{
   listFilesInDirectory(fs1, dirname, levels, onFile);
}

/// @brief Affiche le contenu d’un répertoire et sous-répertoires.
/// @param fs Instance du FS.
/// @param dirname Nom du répertoire.
/// @param levels Profondeur de récursion.
/// @param onFile Callback de lecture de fichier.
/// @param onError Callback d'erreur.
void listFilesInDirectory(fs::FS &fs, const String &dirname, uint8_t levels, FileCallback onFile)
{
   checkIfMounted_HAL();
   Serial.printf("Listing directory: %s\r\n", dirname);

   File root = fs.open(dirname);
   if (!root)
   {
      throw std::runtime_error(("ListDir: failed to open directory: " + dirname).c_str());
   }
   if (!root.isDirectory())
   {
      root.close();
      throw std::runtime_error(("ListDir: not a directory: " + dirname).c_str());
   }

   File file = root.openNextFile();
   while (file)
   {
      if (file.isDirectory())
      {
         Serial.print("  DIR : ");
         Serial.println(file.name());
         if (levels)
         {
            listFilesInDirectory(fs, file.path(), levels - 1, onFile);
         }
      }
      else
      {
         FileInfo fileInfo;
         fileInfo.name = file.name();
         fileInfo.path = file.path();
         fileInfo.size = file.size();
         file.close();
         
         if (onFile)
         {
            onFile(fileInfo);
         }
      }

      file = root.openNextFile();
   }
   root.close();
}

void testFileIO(const String &path)
{
   testFileIO(fs1, path);
}

/// @brief Teste les performances d’écriture/lecture sur un fichier.
/// @param path Chemin du fichier.
void testFileIO(fs::FS &fs, const String &path)
{
   checkIfMounted_HAL();
   Serial.printf("Testing file I/O with %s\r\n", path.c_str());

   static uint8_t buf[512];
   size_t len = 0;
   File file = fs.open(path, FILE_WRITE);
   if (!file)
   {
      Serial.println("TestFile: failed to open file for writing");
      return;
   }

   size_t i;
   Serial.print("TestFile: writing");
   uint32_t start = millis();
   for (i = 0; i < 2048; i++)
   {
      if ((i & 0x001F) == 0x001F)
      {
         Serial.print(".");
      }
      file.write(buf, 512);
   }
   Serial.println("");
   uint32_t end = millis() - start;
   // unsigned long end = millis() − start;
   Serial.printf(" %u bytes written in %u ms\r\n", 2048 * 512, end);
   file.close();

   file = fs.open(path);
   start = millis();
   end = start;
   i = 0;
   if (file && !file.isDirectory())
   {
      len = file.size();
      size_t flen = len;
      start = millis();
      Serial.print("TestFile: reading");
      while (len)
      {
         size_t toRead = len;
         if (toRead > 512)
         {
            toRead = 512;
         }
         file.read(buf, toRead);
         if ((i++ & 0x001F) == 0x001F)
         {
            Serial.print(".");
         }
         // len −= toRead;
         len = len - toRead;
      }
      Serial.println("");
      end = millis() - start;
      Serial.printf(" %u bytes read in %u ms\r\n", flen, end);
      file.close();
   }
   else
   {
      Serial.println(" failed to open file for reading");
   }
}
