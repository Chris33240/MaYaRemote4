/*
#include "applicationInternal/micro_miniz.h"
#include "helpers/base64Utils.h"

//utilisation "officielle" micro-miniz
void test1()
{
    auto original_data = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque metus neque, consequat nec urna quis, lacinia ornare felis. Sed pulvinar metus augue, sed sagittis ligula luctus nec. Integer lectus augue, molestie id leo sit amet, accumsan aliquam erat. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Etiam luctus felis eu tincidunt ornare. Sed ultricies euismod nisi. Morbi tellus odio, dapibus et tincidunt ut, consequat ut dolor. Phasellus bibendum, nulla eget laoreet tincidunt, leo magna sodales arcu, vitae viverra diam urna sed nisi. Sed at tortor congue, rutrum dui ac, aliquet eros. Pellentesque nulla dolor, rhoncus at efficitur non, rutrum a orci.";
    const mz_ulong original_length = sizeof(original_data);

    // Compress the data
    mz_ulong compressed_len = mz_compressBound(original_length);
    auto compressed_data = new byte[compressed_len];
    if (compress(compressed_data, &compressed_len, (unsigned char *)original_data, original_length) != MZ_OK)
        abort();

    // Uncompress
    auto uncompressed_data = new byte[original_length]; // Note buffer can/should be larger!
    mz_ulong uncompressed_len = original_length;
    if (uncompress(uncompressed_data, &uncompressed_len, compressed_data, compressed_len) != MZ_OK)
        abort();

    if (strcmp(original_data, (const char *)uncompressed_data) != 0)
        abort();

    delete[] uncompressed_data;
    delete[] compressed_data;
}

// --- Compression (utilisation officielle micro-miniz) ---
// IN:  data (const char*) et original_length (mz_ulong)
// OUT: vector<uint8_t> compressé (vide si erreur)
vector<uint8_t> compressZlib2(const char* data, mz_ulong original_length) {
  mz_ulong compressed_len = mz_compressBound(original_length);
  vector<uint8_t> compressed(compressed_len);

  int rc = compress(compressed.data(), &compressed_len,
                    reinterpret_cast<const unsigned char*>(data), original_length);
  if (rc != MZ_OK) {
    Serial.printf("compress() failed: %d\n", rc);
    return {}; // échec
  }

  compressed.resize(compressed_len);
  return compressed;
}

// --- Décompression (utilisation officielle micro-miniz) ---
// IN:  src (pointer), src_len (taille compressée), expected_len (taille originale attendue)
// OUT: vector<uint8_t> décompressé (vide si erreur)
vector<uint8_t> decompressZlib2(const uint8_t* src, mz_ulong src_len, mz_ulong expected_len) {
  mz_ulong uncompressed_len = expected_len;
  vector<uint8_t> uncompressed(uncompressed_len);

  int rc = uncompress(uncompressed.data(), &uncompressed_len, src, src_len);
  if (rc != MZ_OK) {
    Serial.printf("uncompress() failed: %d\n", rc);
    return {}; // échec
  }

  uncompressed.resize(uncompressed_len);
  return uncompressed;
}

// ----------------------------
// Encapsulation avec préfixe taille (LE) + Base64
// ----------------------------

// Compose: [4 bytes LE original_length] + [compressed bytes] --> Base64 String
String compressAndEncode(const String& input) {
  mz_ulong original_length = (mz_ulong)input.length(); // taille sans '\0'
  auto compressed = compressZlib2(input.c_str(), original_length);
  if (compressed.empty()) return String();

  // Payload size = 4 + compressed.size()
  size_t payload_len = 4 + compressed.size();
  vector<uint8_t> payload(payload_len);

  // write little-endian uint32_t (original_length)
  uint32_t len32 = static_cast<uint32_t>(original_length);
  payload[0] = (uint8_t)(len32 & 0xFF);
  payload[1] = (uint8_t)((len32 >> 8) & 0xFF);
  payload[2] = (uint8_t)((len32 >> 16) & 0xFF);
  payload[3] = (uint8_t)((len32 >> 24) & 0xFF);

  // append compressed bytes
  memcpy(payload.data() + 4, compressed.data(), compressed.size());

  // Base64 encode (using your Base64Utils.h)
  return base64Encode(payload.data(), payload.size());
}

// Décodage Base64 puis lecture length LE puis décompression
// Retourne String vide en cas d'erreur
String decodeAndDecompress(const String& b64) {
  // decode base64 -> payload
  vector<uint8_t> payload = base64Decode(b64);
  if (payload.size() < 4) {
    Serial.println("decodeAndDecompress: payload too small");
    return String();
  }

  // read little-endian uint32
  uint32_t len32 = (uint32_t)payload[0]
                 | ((uint32_t)payload[1] << 8)
                 | ((uint32_t)payload[2] << 16)
                 | ((uint32_t)payload[3] << 24);
  mz_ulong original_length = (mz_ulong)len32;

  // compressed region
  const uint8_t* comp_ptr = payload.data() + 4;
  mz_ulong comp_len = (mz_ulong)(payload.size() - 4);

  auto decompressed = decompressZlib2(comp_ptr, comp_len, original_length);
  if (decompressed.empty()) {
    Serial.println("decodeAndDecompress: decompression failed");
    return String();
  }

  // create String from decompressed bytes
  return String((const char*)decompressed.data(), decompressed.size());
}

// ----------------------------
// Test round-trip
// ----------------------------
void test_miniz() {
  const String input = R"({"IR_2_0xC800F040C":{"commandHandler":"3","requestType":"WRITE"}})";

  Serial.printf("Input (%u): %s\n", (unsigned)input.length(), input.c_str());

  String b64 = compressAndEncode(input);
  if (b64.length() == 0) {
    Serial.println("compressAndEncode failed!");
    return;
  }
  Serial.printf("Encoded Base64 (len=%u)\n", (unsigned)b64.length());

  String output = decodeAndDecompress(b64);
  if (output.length() == 0) {
    Serial.println("decodeAndDecompress failed!");
    return;
  }

  bool ok = (output == input);
  Serial.printf("Round-trip OK? %s\n", ok ? "true" : "false");
}







// ===============================
//  Helpers Base64
// ===============================

String byteArrayToBase64(const vector<uint8_t>& compressed) {
    return base64Encode(compressed.data(), compressed.size());
}

vector<uint8_t> base64ToByteArray(const String& b64) {
    //return base64Decode(b64);
    return base64Decode(b64);
}
*/
