#include "filesystem_spiffs_hal_esp32.h"

namespace nsSPIFFS
{
    /// @brief Formate le système de fichiers SPIFFS.
    /// @note Ne pas utiliser en production !
    /// @warning Cette fonction est commentée pour éviter la perte de données.
    void format()
    {
        // DON'T USE FORMAT PLEASE !!!
        // if (!SPIFFS.format()) {
        // Serial.println("An Error has occurred while formating SPIFFS");
        // throw std::runtime_error("An error has occurred while formating SPIFFS");
        //}
    }

    /// @brief Monte le système de fichiers SPIFFS.
    /// @return true si le montage réussit.
    /// @throws std::runtime_error si le montage échoue.
    bool mount()
    {
        bool isMounted;
#ifdef FORMAT_FILE_SYSTEM_IF_FAILED
        isMounted = SPIFFS.begin(FORMAT_FILE_SYSTEM_IF_FAILED);
#endif
        if (!isMounted)
        {
            Serial.println("An Error has occurred while mounting SPIFFS");
            throw std::runtime_error("An error has occurred while mounting SPIFFS");
        }
        return isMounted;
    }

    /// @brief Démonte le système de fichiers SPIFFS et libère les ressources.
    void unMount()
    {
        SPIFFS.end();
    }

    /// @brief Affiche des informations sur l’espace total et utilisé du FS SPIFFS.
    void printFreeSpace()
    {
        unsigned int totalBytes = SPIFFS.totalBytes();
        unsigned int usedBytes = SPIFFS.usedBytes();
        Serial.println("========== File System Info ===========");
        Serial.println("totalBytes : " + totalBytes);
        Serial.println("usedBytes : " + usedBytes);
    }

    /// @brief Récupère la quantité d’octets utilisés dans SPIFFS.
    /// @return Nombre d’octets utilisés.
    unsigned int getUsedBytes()
    {
        return SPIFFS.usedBytes();
    }

    /// @brief Récupère la capacité totale du FS SPIFFS.
    /// @return Nombre total d’octets disponibles.
    unsigned int getTotalBytes()
    {
        return SPIFFS.totalBytes();
    }

}

/*
"r" : lecture seule
"r+": lecture + écriture. Le curseur est positionné au début du fichier.
"w" : write (écriture). Lecontenu existant est supprimé. Le fichier est crée si il n'existe pas.
"w+": Ouvre le fichier en lecture et à l'écriture. Le fichier est crée si il n'existe pas. sinon il est tronqué aprés l'écriture.
        Le curseur est positionné au début du fichier, il faut déplacer le curseur si necessaire.
"a" : (append) ouvre un fichier en ajout de données. Le fichier est crée si il n'existe pas.
        Le curseur est positionné à la fin du fichier si il existe déja.
"a+": (append) ouvre un fichier en ajout de données. Le fichier est crée si il n'existe pas.
        Le curseur est positionné au début du fichier en lecture et à la fin du fichier pour l'écriture (ajout).
*/

/* Positionnement du curseur
File1.seek(40); Curseur à la position 40.
File1.seek(40, SeekSet); Curseur déplacé de 40 octets depuis le début.
File1.seek(40, SeekCur); Curseur déplacé de 40 octets depuis la derniere position du curseur.
File1.seek(-4, SeekEnd); Curseur déplacé de -4 octets à partir de la fin du fichier.
*/

//     // LECTURE
// std::string filename = "/datas.json";
// File file1 = SPIFFS.open(filename.c_str(), "r");
// unsigned tailleFichier = file1.size();
// unsigned int fichierPosition = file1.position(); // Position = o si le fichier n'a pas été lu.
// String jsonString = file1.readString(); // Lecture d'une chaine de caractères.
// // Affichage des données du fichier :
// Serial.println(("Affichage du contenu du fichier: " + filename).c_str());
// Serial.println(jsonString);
// // Autre maniere de lire (caractère par caractère) :
// while (file1.available()) {
//     Serial.write(file1.read());
// }
// file1.close();

// // LECTURE + ECRITURE
// File file2 = SPIFFS.open(filename.c_str(), "w");
// int value1;
// for (int i=1; i<=1000; i++) {
//     value1 +=1;
//     file2.println(value1); // 'print()' ou 'println()' permet d'écrire dans le fichier. La méthode 'write()' permet d'écrire un seule entier 8 bits non signé à la fin du fichier.
// }
// file2.close();