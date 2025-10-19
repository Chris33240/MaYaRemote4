#include "commands_io.h"
#include "PinDefinitionsAndMore.h"
#include "setup.h"
#include "commands_json.h"
#include "io.h"

/// @brief Met à jour les données de commandes à partir des fchiers.
/// @param commands Map des commandes existantes.
/// @param errorCount Compteur d'erreurs pour suivre les fichiers corrompus ou manquants.
void updateCommandsFromFiles(std::map<std::string, commandData2> &commands, int &errorCount)
{
    Serial.println("");
    Serial.printf("Updating commands from files.\r\n");
    for (const auto &pair : commands)
    {
        std::string commandName = pair.first;
        commandData2 data;
        data = pair.second;

        try
        {
            std::pair<std::string, commandData2> pair = loadCommand(commandName);
            data = pair.second;
            commands[commandName] = data; // Overwriting command if the key already exist.
        }
        catch (const std::exception &e)
        {
            //Serial.print(F("File not found or corrupted, trying to create new file command: "));
            //Serial.println(commandName.c_str());
            //saveCommand(commandName, data);
            //Serial.print(F("File not found or corrupted, new file command has been created sucessfuly: "));
            //Serial.println(commandName.c_str());
            errorCount++;
        }
    }
}

/// @brief Crée le nom de fichier pour une commande.
/// @param commandName Nom de la commande.
/// @return Nom de fichier avec extension ".cde".
String makeFileCommandName(std::string commandName)
{
    return ("" + commandName + "." + COMMAND_EXTENSION_FILENAME).c_str();
}

/// @brief Charge une commande depuis le fichier avec montage/Démontage du système de fichier.
/// @param commandName Nom de la commande.
/// @return Paire <nom de commande, données>.
std::pair<std::string, commandData2> loadCommand(const std::string &commandName)
{
    String fileName = makeFileCommandName(commandName);
    String path = Io::combinePath("", fileName);
    fsMount2();
    const std::pair<std::string, commandData2> &pair = loadCommand3(path);
    fsUnMount2();
    return pair;
}

/// @brief Charge une commande depuis un chemin complet.
/// @param path Chemin complet du fichier de commande.
/// @return Paire <nom de commande, données>.
std::pair<std::string, commandData2> loadCommand3(const String &path)
{
    // String lower = path;
    // lower.toLowerCase();
    //  Le system de fichier SPIFFS ou LittleFS est sensible à la casse.
    if (!path.endsWith(String(".") + COMMAND_EXTENSION_FILENAME))
    {
        throw std::runtime_error(("Invalid file extension (expected ." + std::string(COMMAND_EXTENSION_FILENAME) + ") : " + std::string(path.c_str())).c_str());
    }
    String jsonString = readFile2(path);
    auto pair = deserializeCommand(jsonString);
    // Vérifiez si l'objet désérialisé est valide
    if (isValidCommand(pair))
    {
        return pair;
    }
    else
    {
        throw std::runtime_error(("[Loading] : Invalid deserialized object " + path).c_str());
    }
}

/// @brief Vérifie si une commande est valide.
/// @param pair Paire <nom de commande, données>.
/// @return true si la commande est valide, false sinon.
bool isValidCommand(const std::pair<std::string, commandData2> &pair)
{
    // Vérification du nom de la commande
    if (pair.first.empty())
    {
        return false;
    }

    // Vérification de la structure commandData2
    const commandData2 &data = pair.second;

    // Vérification de commandHandler (à adapter selon sa définition)
    // if (!isValidCommandHandler(data.commandHandler)) {
    //     return false;
    // }

    // Vérification de requestType
    if (data.requestType.empty())
    {
        return false;
    }

    // Vérification de commandPayloads
    // if (data.commandPayloads.empty()) {
    //     return false;
    // }

    return true;
}

/// @brief Sauvegarde une commande avec montage/Démontage du système de fichier.
/// @param commandName Nom de la commande.
/// @param command Données de la commande.
void saveCommand(const std::string commandName, commandData2 command)
{
    String fileName = makeFileCommandName(commandName);
    String path = Io::combinePath("", fileName);
    saveCommand2(commandName, command, path);
}

/// @brief Sauvegarde une commande avec gestion du chemin.
/// @param commandName Nom de la commande.
/// @param command Données de la commande.
/// @param path Chemin complet du fichier.
void saveCommand2(const std::string commandName, commandData2 command, const String &path)
{
    //String jsonString = serializeCommand(commandName, command, SAVE_COMMAND_JSONPRETTY, false);
    String jsonString = serializeCommandWithPayloads(commandName, command, SAVE_COMMAND_JSONPRETTY);
    try
    {

#if (ENABLED_IO_FILESYSTEM && ENABLED_IO_FILESYSTEM_SAVING)
        fsMount2();
        writeFile2(jsonString, path);
        fsUnMount2();
#endif
        sendBleNotify("Saving file sucess");
    }
    catch (const std::exception &e)
    {
        // nothing to do
    }
}

/// @brief Supprime un fichier de commande avec montage/Démontage du système de fichier.
/// @param commandName Nom de la commande à supprimer.
void deleteCommandFile(const std::string commandName)
{
    String fileName = makeFileCommandName(commandName);
    String path = Io::combinePath("", fileName);
    try
    {
#if (ENABLED_IO_FILESYSTEM && ENABLED_IO_FILESYSTEM_SAVING)
        fsMount2();
        deleteFile2(path);
        fsUnMount2();
#endif
        sendBleNotify("Delete file sucess");
    }
    catch (const std::exception &e)
    {
        // nothing to do
    }
}

/// @brief Charge tous les fichiers de commandes dans un répertoire avec montage/Démontage du système de fichier.
/// @param commands Map des commandes.
/// @param errorCount Compteur d’erreurs.
/// @param dirname Répertoire de départ. Defaut = "/" (racine)
/// @param levels Profondeur récursive. Défaut = 0 (aucun niveau)
void loadCommandFiles(std::map<std::string, commandData2> &commands, int &errorCount, const String &dirname, uint8_t levels)
{
    Serial.println();
    Serial.printf("Loading command files in directory: %s\r\n", dirname);

    try
    {
        // 🔹 1. Collecter NOUVEAU les fichiers AVANT traitement
        std::vector<FileInfo> fileList;
        fsMount2();
        listFilesInDirectory(dirname, levels, [&](const FileInfo &fileInfo)
                             {
            // Ne pas faire d'opératon sur les fichiers à l'interieur du callback.
            if (fileInfo.path.endsWith(String(".") + COMMAND_EXTENSION_FILENAME)) {
                // Seul les fichiers command sont chargés
                fileList.push_back(fileInfo);
            } });
        fsUnMount2();

        // 🔹 2. Traiter les fichiers APRÈS la collecte
        for (auto &fileInfo : fileList)
        {
            try
            {
                fsMount2();
                auto pair = loadCommand3(fileInfo.path);
                fsUnMount2();

                commands[pair.first] = pair.second;

                Serial.print(F("Fichier chargé: "));
                Serial.println(fileInfo.path);
                Serial.println(fileInfo.name);
            }
            catch (const std::exception &e)
            {
                Serial.print(F("Erreur lors du chargement du fichier: "));
                Serial.println(fileInfo.path);
                Serial.print(F("Message: "));
                Serial.println(e.what());

                errorCount++;

                // Suppression du fichier corrompu
                try
                {
                    fsMount2();
                    deleteFile2(fileInfo.path);
                    fsUnMount2();
                    Serial.print(F("Fichier supprimé: "));
                    Serial.println(fileInfo.path);
                }
                catch (...)
                {
                    Serial.print(F("Impossible de supprimer le fichier: "));
                    Serial.println(fileInfo.path);
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        Serial.print(F("Erreur d'accès au répertoire: "));
        Serial.println(dirname);
        Serial.print(F("Message: "));
        Serial.println(e.what());
        errorCount++;
    }
}

/// @brief Charge toutes les clés contenues dans les fichiers de commande.
/// @param commandsKeys Liste des noms de commandes.
/// @param errorCount Compteur d’erreurs.
/// @param dirname Répertoire de départ. Defaut = "/" (racine)
/// @param levels Profondeur récursive. Défaut = 0 (aucun niveau)
void loadCommandsKeysFromFiles(std::set<std::string> &commandsKeys, int &errorCount, const String &dirname, uint8_t levels)
{
    Serial.println();
    Serial.printf("Loading command files in directory: %s\r\n", dirname);

    try
    {
        fsMount2();
        // 🔹 1. Collecter NOUVEAU les fichiers AVANT traitement
        std::vector<FileInfo> fileList;
        listFilesInDirectory(dirname, levels, [&](const FileInfo &fileInfo)
                             {
            // Ne pas faire d'opératon sur les fichiers à l'interieur du callback.
            if (fileInfo.path.endsWith(String(".") + COMMAND_EXTENSION_FILENAME)) {
                // Seul les fichiers command sont chargés
                fileList.push_back(fileInfo);
            } });
        fsUnMount2();

        // 🔹 2. Traiter les fichiers APRÈS la collecte
        for (auto &fileInfo : fileList)
        {
            try
            {
                fsMount2();
                auto pair = loadCommand3(fileInfo.path);
                fsUnMount2();
                // commands[pair.first] = pair.second;
                commandsKeys.insert(pair.first);

                Serial.print(F("Clé chargé depuis le fichier: "));
                Serial.println(fileInfo.path);
                Serial.println(fileInfo.name);
            }
            catch (const std::exception &e)
            {
                Serial.print(F("Erreur lors du chargement de la clé: "));
                Serial.println(fileInfo.path);
                Serial.print(F("Message: "));
                Serial.println(e.what());

                errorCount++;

                // Suppression du fichier corrompu
                try
                {
                    fsMount2();
                    deleteFile2(fileInfo.path);
                    fsUnMount2();
                    Serial.print(F("Fichier supprimé: "));
                    Serial.println(fileInfo.path);
                }
                catch (...)
                {
                    Serial.print(F("Impossible de supprimer le fichier: "));
                    Serial.println(fileInfo.path);
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        Serial.print(F("Erreur d'accès au répertoire: "));
        Serial.println(dirname);
        Serial.print(F("Message: "));
        Serial.println(e.what());
        errorCount++;
    }
}

/// @brief Supprime tous les fichiers dans un répertoire avec montage/Démontage du système de fichier.
/// @param errorCount Compteur d’erreurs.
/// @param dirname Répertoire de départ. Defaut = "/" (racine)
/// @param levels Profondeur récursive. Défaut = 0 (aucun niveau)
void deleteAllFiles(int &errorCount, const String &dirname, uint8_t levels)
{
    fsMount2();
    Serial.println();
    Serial.printf("Delete all files in directory: %s\r\n", dirname);
    try
    {
        // deleteAllFiles2(dirname, levels, errorCount);
        listFilesInDirectory(dirname, levels, [&](const FileInfo &fileInfo)
                             { deleteFile(fileInfo, errorCount); });
    }
    catch (const std::exception &e)
    {
        Serial.print(F("Erreur accès répertoire: "));
        Serial.println(dirname);
        Serial.print(F("Message: "));
        Serial.println(e.what());
        errorCount++;
    }
    fsUnMount2();
}

void deleteFile(const FileInfo &fileInfo, int &errorCount)
{
    try
    {
        // fsMount2();
        deleteFile2(fileInfo.path);
        // fsUnMount2();
        Serial.print(F("Fichier supprimé: "));
        Serial.println(fileInfo.path);
    }
    catch (const std::exception &e)
    {
        Serial.print(F("Erreur suppression fichier: "));
        Serial.println(fileInfo.path);
        Serial.print(F("Message: "));
        Serial.println(e.what());
        errorCount++;
    }
}

/// @brief Liste les fichiers d’un répertoire avec montage/Démontage du système de fichier.
/// @param dirname Répertoire de départ. Defaut = "/" (racine)
/// @param levels Profondeur récursive. Défaut = 0 (aucun niveau)
void printFilesInfo(const String &dirname, uint8_t levels)
{
    fsMount2();
    Serial.println();
    Serial.printf("Listing directory: %s\r\n", dirname);

    try
    {
        listFilesInDirectory(dirname, levels, [&](const FileInfo &fileInfo)
                             { printFileInfo(fileInfo); });
        // listDir2(dirname, levels, printFileInfo);
    }
    catch (const std::exception &e)
    {
        Serial.print(F("Erreur accès répertoire: "));
        Serial.println(dirname);
        Serial.print(F("Message: "));
        Serial.println(e.what());
    }

    fsUnMount2();
}

void printFileInfo(const FileInfo &fileInfo)
{
    // Ici on différencie fichiers et répertoires si besoin
    // Dans listFilesInDirectory, les répertoires peuvent être indiqués par un "/" à la fin
    if (!fileInfo.name.isEmpty())
    {
        Serial.print("  FILE: ");
        Serial.print(fileInfo.name);
        // Optionnel: récupérer la taille du fichier depuis le HAL si nécessaire
        // Serial.print("\tSIZE: ...");
        Serial.print("\tSIZE: ");
        Serial.print(fileInfo.size);
        Serial.println();
    }
    else
    {
        // Pour info, on pourrait afficher les répertoires différemment
        Serial.print("  DIR : ");
        Serial.println(fileInfo.path);
    }
}

// NON UTILISE
/// @brief Liste les fichiers d’un répertoire avec montage/Démontage du système de fichier.
/// @param dirname Répertoire de départ.
/// @param levels Profondeur récursive.
/// @param onFileAction Callback pour les opérations sur le fichier.
void listDir2(const String &dirname, uint8_t levels, FileCallback onFileAction)
{
    listFilesInDirectory(dirname, levels, [&](const FileInfo &fileInfo)
                         { onFileAction(fileInfo); });
}

/*
/// @brief Charge tous les fichiers de commandes dans un répertoire avec montage/Démontage du système de fichier.
/// @param errorCount Compteur d’erreurs.
/// @param dirname Répertoire de départ. Defaut = "/" (racine)
/// @param levels Profondeur récursive. Défaut = 0 (aucun niveau)
void updateCommandFiles(int &errorCount, const String &dirname, uint8_t levels)
{
    Serial.println();
    Serial.printf("Updating command files in directory: %s\r\n", dirname);

    try
    {
        // 🔹 1. Collecter NOUVEAU les fichiers AVANT traitement
        std::vector<FileInfo> fileList;
        fsMount2();
        listFilesInDirectory(dirname, levels, [&](const FileInfo &fileInfo)
                             {
            // Ne pas faire d'opératon sur les fichiers à l'interieur du callback.
            if (fileInfo.path.endsWith(String(".") + COMMAND_EXTENSION_FILENAME)) {
                // Seul les fichiers command sont chargés
                fileList.push_back(fileInfo);
            } });
        fsUnMount2();

        // 🔹 2. Traiter les fichiers APRÈS la collecte
        for (auto &fileInfo : fileList)
        {
            try
            {
                fsMount2();
                auto pair = loadCommand3(fileInfo.path);
                fsUnMount2();
                std::string commandName = pair.first;
                commandData2 commandData = pair.second;
                //saveCommand(commandName, commandData);
                fsMount2();
                saveCommand2(commandName, commandData, fileInfo.path);
                fsUnMount2();
                //commands[pair.first] = pair.second;

                Serial.print(F("Fichier mis à jour: "));
                Serial.println(fileInfo.path);
                Serial.println(fileInfo.name);
            }
            catch (const std::exception &e)
            {
                Serial.print(F("Erreur lors de la mise à jour du fichier: "));
                Serial.println(fileInfo.path);
                Serial.print(F("Message: "));
                Serial.println(e.what());

                errorCount++;
            }
        }
    }
    catch (const std::exception &e)
    {
        Serial.print(F("Erreur d'accès au répertoire: "));
        Serial.println(dirname);
        Serial.print(F("Message: "));
        Serial.println(e.what());
        errorCount++;
    }
}
*/

