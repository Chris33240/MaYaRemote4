#include "infrared_utils.h"
#include <IRac.h>

/// Convertit une valeur booléenne en chaîne de caractères.
///
/// @param value Booléen à convertir.
/// @return "true" si `value` est vrai, sinon "false".
String boolToString(const bool value)
{
  if (value)
  {
    return "true";
  }
  return "false";
}

/// Retourne le nom du protocole sous forme de chaîne.
///
/// ⚠️ Cette fonction est obsolète (remplacée par getPayloads).
///
/// @param protocol Type de protocole décodé.
/// @return Chaîne contenant le nom du protocole, ou "UNKNOWN" si non reconnu.
String protocolToString(const decode_type_t protocol)
{
  String result = "";
  result.reserve(30); // Taille du nom de protocole la plus longue
  if (protocol > kLastDecodeType || protocol == decode_type_t::UNKNOWN)
  {
    result = kUnknownStr;
  }
  else
  {
    auto *ptr = reinterpret_cast<const char *>(kAllProtocolNamesStr);
    for (uint16_t i = 0; i <= protocol && std::strlen(ptr); i++)
    {
      if (i == protocol)
      {
        // result = FPSTR(ptr);
        result = String(ptr);
        break;
      }
      ptr += std::strlen(ptr) + 1;
    }
  }
  return result;
}

/// Indique si un protocole possède un état (AC state).
///
/// @param protocol Type de protocole.
/// @return "true" si le protocole gère un état, sinon "false"
String hasACStateToString(const decode_type_t protocol)
{
  const bool hasState = hasACState(protocol);
  return boolToString(hasState);
}

/// Convertit le tampon brut (raw buffer) d'un décodage en chaîne de caractères.
///
/// @param results Pointeur vers les résultats du décodage.
/// @return Chaîne représentant le contenu du `rawbuf`.
String rawBufToString(const decode_results *const results)
{
  String output = "";
  output += F("{"); // Start declaration

  // Dump data
  for (uint16_t i = 1; i < results->rawlen; i++)
  {
    uint32_t usecs;
    for (usecs = results->rawbuf[i] * kRawTick; usecs > UINT16_MAX;
         usecs -= UINT16_MAX)
    {
      output += uint64ToString(UINT16_MAX);
      if (i % 2)
        output += F(", 0,  ");
      else
        output += F(",  0, ");
    }
    output += uint64ToString(usecs, 10);
    if (i < results->rawlen - 1)
      output += kCommaSpaceStr; // ',' not needed on the last one
    if (i % 2 == 0)
      output += ' '; // Extra if it was even.
  }

  // End declaration
  output += F("}");
  return output;
}

std::vector<uint16_t> stringToRawBuf(const std::string &valuesStr)
{
  std::vector<uint16_t> rawBuffer;
  std::stringstream ss(valuesStr);
  std::string temp;

  while (std::getline(ss, temp, ','))
  {
    temp.erase(0, temp.find_first_not_of(' '));
    temp.erase(temp.find_last_not_of(' ') + 1);
    if (!temp.empty())
    {
      rawBuffer.push_back(static_cast<uint16_t>(std::stoi(temp)));
    }
  }

  return rawBuffer;
}

/// @brief Convertit une chaîne contenant des valeurs hexadécimales en un tableau dynamique d'octets.
/// Cette fonction analyse une chaîne de la forme "0x1A, 0x0F, 0x3C" (avec ou sans espaces)
/// et extrait chaque valeur précédée de "0x" ou "0X" pour la convertir en uint8_t.
/// @param input Chaîne contenant des valeurs hexadécimales séparées par des virgules.
///              Exemples valides : "0x1A, 0x0F, 0x3C" ou "0XAA,0XBB,0xCC"
/// @return std::vector<uint8_t> Vecteur contenant chaque octet décodé dans l'ordre d'apparition.
/// @note Les valeurs non préfixées par "0x" ou "0X" sont ignorées.
std::vector<uint8_t> hexStringToByteArray(const std::string &input)
{
  std::vector<uint8_t> buffer;
  std::string temp;

  for (size_t i = 0; i < input.size(); i++)
  {
    if (input[i] == '0' && i + 1 < input.size() && (input[i + 1] == 'x' || input[i + 1] == 'X'))
    {
      i += 2; // skip "0x"
      temp.clear();

      while (i < input.size() && isxdigit(input[i]))
      {
        temp += input[i];
        i++;
      }

      buffer.push_back(static_cast<uint8_t>(std::stoul(temp, nullptr, 16)));
    }
  }

  return buffer;
}

/// @brief Convertit une chaîne hexadécimale en uint64_t.
/// Cette fonction est l'inverse de ValueToStringIfHasState().
/// Elle permet de recharger une valeur simple (protocoles sans état AC).
/// @param hexStr Chaîne contenant la valeur hexadécimale (ex: "1A3F" ou "0x1A3F").
/// @return uint64_t Valeur convertie. Retourne 0 en cas d'erreur ou chaîne vide.
uint64_t stringToUint64(const String &hexStr)
{
  String clean = hexStr;

  // Enlever le prefixe "0x" ou "0X" si présent
  if (clean.startsWith("0x") || clean.startsWith("0X"))
  {
    clean = clean.substring(2);
  }

  uint64_t value = 0;
  for (int i = 0; i < clean.length(); i++)
  {
    char c = clean[i];
    if (c >= '0' && c <= '9')
    {
      value = (value << 4) | (c - '0');
    }
    else if (c >= 'A' && c <= 'F')
    {
      value = (value << 4) | (c - 'A' + 10);
    }
    else if (c >= 'a' && c <= 'f')
    {
      value = (value << 4) | (c - 'a' + 10);
    }
    else
    {
      break; // Caractère invalide
    }
  }

  return value;
}

/// Retourne la valeur décodée en hexadécimal uniquement si le protocole n’a pas d’état AC.
///
/// @param results Pointeur vers les résultats du décodage.
/// @return Valeur en hexadécimal si applicable, sinon chaîne vide.
String ValueToStringIfNotHasState(const decode_results *const results)
{
  // Only return the value if the decode_type doesn't have an A/C state.
  if (!hasACState(results->decode_type))
  {
    return uint64ToString(results->value, 16);
  }
  return "";
}

/// Retourne la taille corrigée des données brut sous forme de chaîne héxadécimale.
///
/// @param results Pointeur vers les résultats du décodage.
/// @return taille des données brut en héxadécimal.
String lengthToString(const decode_results *const results)
{
  const uint16_t length = getCorrectedRawLength(results);
  return uint64ToString(length, 10);
}

/// Convertit l’état AC décodé en tableau hexadécimal.
///
/// @param results Pointeur vers les résultats du décodage.
/// @return Chaîne représentant les octets de l’état AC sous la forme {0x.., 0x..}.
String ACStateCodeToString(const decode_results *const results)
{
  String output = "";
  if (results->decode_type != UNKNOWN)
  {
    //if (hasState) {
#if DECODE_AC
      uint16_t nbytes = ceil(static_cast<float>(results->bits) / 8.0);
      // output += F("uint8_t state[");
      // output += uint64ToString(nbytes);
      output += F("{");
      for (uint16_t i = 0; i < nbytes; i++)
      {
        output += F("0x");
        if (results->state[i] < 0x10) output += '0';
        output += uint64ToString(results->state[i], 16);
        if (i < nbytes - 1) output += kCommaSpaceStr;
      }
      output += F("}");
#endif // DECODE_AC
    //}
  }
  return output;
}

/// Convertit une adresse en représentation hexadécimale.
///
/// @param address Adresse numérique.
/// @return Adresse au format "0xXXXXXXXX".
String AddressToString(const uint32_t address)
{
  String output;
  // if (address > 0) {
  output = "0x" + uint64ToString(address, 16);
  //}
  return output;
}

/// Convertit une commande en représentation hexadécimale.
///
/// @param command Code de commande.
/// @return Commande au format "0xXXXXXXXX".
String CommandToString(const uint32_t command)
{
  String output;
  // if (command > 0) {
  output = "0x" + uint64ToString(command, 16);
  //}
  return output;
}

/// Convertit une valeur brute en code hexadécimal.
///
/// @param value Valeur numérique.
/// @return Valeur au format "0xXXXXXXXX".
String DataCodeToString(const uint64_t value)
{
  return "0x" + uint64ToString(value, 16);
}