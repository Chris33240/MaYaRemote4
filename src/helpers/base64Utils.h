// Base64Utils.h
// Implémentation Base64 autonome (RFC 4648) — aucun include externe requis.
// - Alphabet standard "A–Z a–z 0–9 + /" avec '=' pour le padding.
// - Aucun retour à la ligne (NO_WRAP).
// Compatible Android Base64.DEFAULT / NO_WRAP.

/*
#pragma once
#include <Arduino.h>
#include <vector>

static const char B64_ALPHABET[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline String base64Encode(const uint8_t* data, size_t len) {
  String out;
  out.reserve(((len + 2) / 3) * 4);
  int val = 0;
  int valb = -6;
  for (size_t i = 0; i < len; i++) {
    val = (val << 8) + data[i];
    valb += 8;
    while (valb >= 0) {
      out += B64_ALPHABET[(val >> valb) & 0x3F];
      valb -= 6;
    }
  }
  if (valb > -6) out += B64_ALPHABET[((val << 8) >> (valb + 8)) & 0x3F];
  while (out.length() % 4) out += '=';
  return out;
}

inline std::vector<uint8_t> base64Decode(const String& input) {
  // Table de décodage
  int T[256];
  for (int i = 0; i < 256; i++) T[i] = -1;
  for (int i = 0; i < 64; i++) T[(uint8_t)B64_ALPHABET[i]] = i;

  std::vector<uint8_t> out;
  out.reserve((input.length() * 3) / 4);
  int val = 0;
  int valb = -8;

  for (size_t idx = 0; idx < input.length(); idx++) {
    unsigned char c = (unsigned char)input[idx];
    if (c == '=') break;             // fin de flux
    int d = T[c];
    if (d == -1) continue;           // ignore espaces/retours à la ligne si jamais
    val = (val << 6) + d;
    valb += 6;
    if (valb >= 0) {
      out.push_back((uint8_t)((val >> valb) & 0xFF));
      valb -= 8;
    }
  }
  return out;
}
*/
