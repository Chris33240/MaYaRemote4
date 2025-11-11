#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <string>
#include <list>

namespace helpers
{
    inline bool IsTrue(const std::string &str)
    {
        return (str == "1" || str == "true" || str == "True");
    }

    inline void printPayloadsContent(std::list<std::string> payloads)
    {
        Serial.println(F("    print payload content: "));
        for (const auto &str : payloads)
        {
            Serial.println(("    -" + str).c_str());
        }
    }

    inline std::string convertStringListToString(std::list<std::string> listOfStrings)
    {
        std::string result;
        for (const auto &word : listOfStrings)
        {
            result += word + ",";
        }
        return result;
    }

    inline std::string longString(int lenght)
    {
        std::string str;
        for (int i = 0; i < lenght; ++i)
        {
            int c = i % 10;
            str += std::to_string(c); // Ex: 0123456789....
        }
        return str;
    }

    inline std::string trimChar(const std::string &str, char c)
    {
        size_t start = 0;
        size_t end = str.size();

        if (!str.empty() && str.front() == c)
            start++;
        if (end > start && str.back() == c)
            end--;

        return str.substr(start, end - start);
    }
    // template <typename T>
    // class EnumParser
    // {
    //     map <string, T> enumMap;
    // public:
    //     EnumParser(){};

    //     T ParseSomeEnum(const string &value)
    //     {
    //         map <string, T>::const_iterator iValue = enumMap.find(value);
    //         if (iValue  == enumMap.end())
    //             throw runtime_error("");
    //         return iValue->second;
    //     }
    // };

    // void printPayloadsContent(std::list<std::string> payloads) {
    //   Serial.println("    print payload content: ");
    //     for (const auto& str : payloads) {
    //       Serial.println(("    -" + str).c_str());
    //     }
    // }

    // bool pinDown(int pinButton)
    // {
    //   static int lastKeyState = HIGH;
    //   if (digitalRead(pinButton) == LOW & digitalRead(pinButton) != lastKeyState)
    //   {
    //     return true;
    //   }
    //   lastKeyState = digitalRead(pinButton);
    //   return false;
    // }

    // bool pinUp(int pinButton)
    // {
    //   static int lastKeyState = LOW;
    //   if (digitalRead(pinButton) == HIGH & digitalRead(pinButton) != lastKeyState)
    //   {
    //     return true;
    //   }
    //   lastKeyState = digitalRead(pinButton);
    //   return false;
    // }

    // bool releaseCommand(String command)
    // {
    //   static String lastcommand = "";
    //   if (command == "" and command != lastcommand) // Command need to be "" (released) to change toogle.
    //   {
    //     lastcommand = command;
    //     return true;
    //   }
    //   lastcommand = command;
    //   return false;
    // }

    inline std::string noNullOrBlank(const std::string *input)
    {
        // Vérifie si le pointeur est nul ou si la chaîne est vide ou composée uniquement d'espaces
        if (!input || input->empty() || std::all_of(input->begin(), input->end(), isspace))
        {
            return ""; // Retourne une chaîne vide
        }
        return *input; // Retourne la chaîne originale si elle est valide
    }

    inline bool hexStringToBytes(const std::string &hex, std::vector<uint8_t> &out)
    {
        out.clear();
        std::string cleanHex = hex;

        // Si la string commence par "0x" ou "0X" → on l’enlève
        if (cleanHex.rfind("0x", 0) == 0 || cleanHex.rfind("0X", 0) == 0)
        cleanHex.erase(0, 2);

        // Enlever le prefixe "0x" ou "0X" si présent
        //if (cleanHex.startsWith("0x") || cleanHex.startsWith("0X"))
        //{
        // cleanHex = cleanHex.substring(2);
        //}

        // Retire "0x", espaces, retour à la ligne, etc.
        //for (char c : hex)
        //{
        //    if (isxdigit(c))
        //        cleanHex += c;
        //}

        // Si nombre impair de caractères -> erreur
        if (cleanHex.size() % 2 != 0)
            return false;

        out.reserve(cleanHex.size() / 2);

        for (size_t i = 0; i < cleanHex.size(); i += 2)
        {
            uint8_t byte = std::strtoul(cleanHex.substr(i, 2).c_str(), nullptr, 16);
            out.push_back(byte);
        }

        return true;
    }

    template <typename T>
    inline T convertToType(const std::string &str, int base = 10)
    {
        try
        {
            // Vérification que T est un type numérique
            static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value,
                          "Le type T doit être un type numérique (entier ou flottant)");

            // Utilisation de std::is_same pour émuler le comportement de if constexpr
            if (std::is_same<T, short>::value)
            {
                return std::stoi(str, nullptr, base);
            }
            else if (std::is_same<T, unsigned short>::value)
            {
                return std::stoi(str, nullptr, base);
            }
            else if (std::is_same<T, int>::value)
            {
                return std::stoi(str, nullptr, base);
            }
            else if (std::is_same<T, uint16_t>::value)
            {
                return std::stoi(str, nullptr, base);
            }
            else if (std::is_same<T, uint32_t>::value)
            {
                return std::stoi(str, nullptr, base);
            }
            else if (std::is_same<T, long>::value)
            {
                return std::stol(str, nullptr, base);
            }
            else if (std::is_same<T, unsigned long>::value)
            {
                return std::stoul(str, nullptr, base);
            }
            else if (std::is_same<T, long long>::value)
            {
                return std::stoll(str, nullptr, base);
            }
            else if (std::is_same<T, unsigned long long>::value)
            {
                return std::stoull(str, nullptr, base);
            }
            else if (std::is_same<T, uint64_t>::value)
            {
                return std::stoull(str, nullptr, base);
            }
            else if (std::is_floating_point<T>::value)
            {
                // Les conversions flottantes ignorent la base
                if (std::is_same<T, float>::value)
                {
                    return std::stof(str);
                }
                else if (std::is_same<T, double>::value)
                {
                    return std::stod(str);
                }
                else if (std::is_same<T, long double>::value)
                {
                    return std::stold(str);
                }
            }

            // Si aucun type ne correspond
            throw std::invalid_argument("Type de conversion non supporté.");
        }
        catch (const std::invalid_argument &)
        {
            Serial.printf("Erreur : chaîne invalide pour la conversion : %s\r\n", str.c_str());
            // std::cerr << "Erreur : chaîne invalide pour la conversion : " << str << std::endl;
            return 0;
        }
        catch (const std::out_of_range &)
        {
            Serial.printf("Erreur : chaîne hors des limites pour le type cible : %s\r\n", str.c_str());
            // std::cerr << "Erreur : chaîne hors des limites pour le type cible : " << str << std::endl;
            return 0;
        }

        return 0; // Retour par défaut (ne devrait pas être atteint)
    }

    /*
    template<typename T>
    T tryToConvertStringToNumber(const std::string& strValue);

    template<>
    inline uint16_t tryToConvertStringToNumber<uint16_t>(const std::string& strValue) {
        try {
            return strValue.empty() ? 0 : static_cast<uint16_t>(std::stoi(strValue));
        } catch (...) {
            return 0;  // ou gérer l'erreur différemment si nécessaire
        }
    }

    template<>
    inline uint32_t tryToConvertStringToNumber<uint32_t>(const std::string& strValue) {
        try {
            return strValue.empty() ? 0 : static_cast<uint32_t>(std::stoi(strValue));
        } catch (...) {
            return 0;  // ou gérer l'erreur différemment si nécessaire
        }
    }

    template<>
    inline uint64_t tryToConvertStringToNumber<uint64_t>(const std::string& strValue) {
        try {
            return strValue.empty() ? 0 : static_cast<uint64_t>(std::stoull(strValue));
        } catch (...) {
            return 0;  // ou gérer l'erreur différemment si nécessaire
        }
    }

    template<>
    inline unsigned short tryToConvertStringToNumber<unsigned short>(const std::string& strValue) {
        try {
            return strValue.empty() ? 0 : std::stoi(strValue);
        } catch (...) {
            return 0;
        }
    }

    template<>
    inline int tryToConvertStringToNumber<int>(const std::string& strValue) {
        try {
            return strValue.empty() ? 0 : std::stoi(strValue);
        } catch (...) {
            return 0;
        }
    }

    template<>
    inline double tryToConvertStringToNumber<double>(const std::string& strValue) {
        try {
            return strValue.empty() ? 0.0 : std::stod(strValue);
        } catch (...) {
            return 0.0;
        }
    }
    */

}

#endif
