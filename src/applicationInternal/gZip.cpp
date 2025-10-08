/*
#include "applicationInternal/gZip.h"
#include "helpers/base64Utils.h"

//   -------------------------
//   Minimal MemoryStream
//   - Implements enough of Arduino Stream to be readable by gzStreamExpander
//   - Wraps a byte vector, keeps a read cursor
//   -------------------------
class MemoryStream : public Stream {
public:
  MemoryStream() : _pos(0) {}
  MemoryStream(const uint8_t* data, size_t len) : _buf(data, data + len), _pos(0) {}

  void setBuffer(const uint8_t* data, size_t len) {
    _buf.assign(data, data + len);
    _pos = 0;
  }

  // Stream interface
  int available() override { return (int)(_buf.size() - _pos); }

  int read() override {
    if (_pos >= _buf.size()) return -1;
    return _buf[_pos++];
  }

  int peek() override {
    if (_pos >= _buf.size()) return -1;
    return _buf[_pos];
  }

  void flush() override {}

  // readBytes is useful; implement explicitly
  size_t readBytes(char *buffer, size_t length) override {
    size_t remain = _buf.size() - _pos;
    size_t toRead = std::min(length, remain);
    if (toRead == 0) return 0;
    memcpy(buffer, _buf.data() + _pos, toRead);
    _pos += toRead;
    return toRead;
  }

  // not used (write side) but implement stub
  size_t write(uint8_t) override { return 0; }

private:
  vector<uint8_t> _buf;
  size_t _pos;
};

//   -------------------------
//   Global context used by static callback
//   (GzUnpacker callback doesn't accept a user pointer in this API)
//   We'll store a pointer to the vector to append decompressed chunks.
//   -------------------------
static vector<uint8_t>* g_decomp_out_ptr = nullptr;

// Signature of stream-writer callback used by GzUnpacker in the library:
// static bool gzStreamWriteCallback( unsigned char* buff, size_t buffsize );
static bool gzip_stream_writer_cb(unsigned char* buff, size_t buffsize) {
  if (!g_decomp_out_ptr) return false;
  if (buff == nullptr || buffsize == 0) return true; // nothing to append
  g_decomp_out_ptr->insert(g_decomp_out_ptr->end(), buff, buff + buffsize);
  return true;
}

//   -------------------------
//   gzipCompressBuffer
//   - compresses input buffer to gzip bytes using LZPacker::compress
//   - returns vector<uint8_t> containing full .gz (RFC1952) or empty on error
//   -------------------------
vector<uint8_t> gzipCompressBuffer(const uint8_t* data, size_t len) {
  if (!data || len == 0) return {};

  uint8_t* outPtr = nullptr;
  // LZPacker::compress(uint8_t* srcBuf, size_t srcBufLen, uint8_t** dstBufPtr) -> size_t compressedSize
  size_t outSize = LZPacker::compress(const_cast<uint8_t*>(data), len, &outPtr);
  if (outSize == 0 || outPtr == nullptr) {
    Serial.println("gzipCompressBuffer: compress failed");
    return {};
  }

  vector<uint8_t> out(outPtr, outPtr + outSize);
  free(outPtr); // docs/examples free the returned buffer
  return out;
}

// -------------------------
//   gzipDecompressBuffer
//   - decompresses gzip bytes (vector) using GzUnpacker streaming API
//   - returns decompressed bytes as vector<uint8_t> (empty on error)
//   -------------------------
vector<uint8_t> gzipDecompressBuffer(const uint8_t* gzData, size_t gzLen) {
  if (!gzData || gzLen == 0) return {};

  // Prepare MemoryStream over source gzip bytes
  MemoryStream mem;
  mem.setBuffer(gzData, gzLen);

  // prepare output accumulator
  vector<uint8_t> out;
  g_decomp_out_ptr = &out; // set global pointer used by callback

  // instantiate unpacker
  GzUnpacker unpacker;

  // register our write callback (signature used in the lib)
  unpacker.setStreamWriter(gzip_stream_writer_cb);

  // call stream expander - many versions have gzStreamExpander(Stream *stream, size_t gz_size = 0)
  // pass gzLen to hint size (some implementations ignore gz_size)
  bool ok = unpacker.gzStreamExpander(&mem, gzLen);
  // cleanup
  unpacker.gzExpanderCleanup();
  g_decomp_out_ptr = nullptr;

  if (!ok) {
    Serial.println("gzipDecompressBuffer: gzStreamExpander failed");
    return {};
  }

  return out;
}

//   -------------------------
//   Base64 helpers (wrappers to your Base64Utils)
//   - base64Encode(const uint8_t*, size_t, bool multiline)
//   - base64Decode(const String&) -> vector<uint8_t>
//   -------------------------
String toBase64(const vector<uint8_t>& data) {
  if (data.empty()) return String();
  return base64Encode(data.data(), data.size());
}

vector<uint8_t> fromBase64(const String& s) {
  if (s.length() == 0) return {};
  return base64Decode(s);
}

//   -------------------------
//   Test round-trip
//   -------------------------
void testGzipRoundTrip() {
  //const String input = R"({"IR_2_0xC800F040C":{"commandHandler":"3","requestType":"WRITE"}})";
  const String input = R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque metus neque, consequat nec urna quis, lacinia ornare felis. Sed pulvinar metus augue, sed sagittis ligula luctus nec. Integer lectus augue, molestie id leo sit amet, accumsan aliquam erat. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Etiam luctus felis eu tincidunt ornare. Sed ultricies euismod nisi. Morbi tellus odio, dapibus et tincidunt ut, consequat ut dolor. Phasellus bibendum, nulla eget laoreet tincidunt, leo magna sodales arcu, vitae viverra diam urna sed nisi. Sed at tortor congue, rutrum dui ac, aliquet eros. Pellentesque nulla dolor, rhoncus at efficitur non, rutrum a orci.)";

  Serial.printf("Input len=%u\n", (unsigned)input.length());

  // compress
  auto gz = gzipCompressBuffer((const uint8_t*)input.c_str(), input.length());
  Serial.printf("gz len=%u\n", (unsigned)gz.size());

  // base64 encode (no newline)
  String b64 = toBase64(gz);
  Serial.printf("b64 len=%u\n", (unsigned)b64.length());

  // base64 decode
  auto gz2 = fromBase64(b64);
  Serial.printf("gz2 len=%u\n", (unsigned)gz2.size());

  // decompress
  auto out = gzipDecompressBuffer(gz2.data(), gz2.size());
  Serial.printf("out len=%u\n", (unsigned)out.size());

  String outputStr((const char*)out.data(), out.size());
  Serial.printf("Round-trip OK? %s\n", (outputStr == input) ? "true" : "false");
}
*/
