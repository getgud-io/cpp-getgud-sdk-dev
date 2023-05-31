#include "pch.h"
#include "Zip.h"
#include "../../pch.h"

namespace GetGudSdk {
Zipper zipper;

// Only this chars are used in base64 encoding
static const std::string base64Chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

namespace {

/**
 * Base64Encode:
 *
 * Encode string as base64, we use this to send zipped string to Getgud
 **/
std::string Base64Encode(unsigned char const* bytes_to_encode,
                         unsigned int in_len) {
  // I know I know, base64 encoding is hard to understand
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] =
          ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] =
          ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (i = 0; (i < 4); i++)
        ret += base64Chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] =
        ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] =
        ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64Chars[char_array_4[j]];

    while ((i++ < 3))
      ret += '=';
  }

  return ret;
}
}  // namespace

/**
 * CompressString:
 *
 * Compress the string, we use it for ActionStream before sending to Getgud
 **/
bool Zipper::CompressString(std::string& inString,
                            std::string& outCompressedString) {
  bool compressedSuccess = false;
  unsigned long compressedDataSize = compressBound(inString.size());
  unsigned char* compressedData = new unsigned char[compressedDataSize];
  int result = compress(compressedData, &compressedDataSize,
                        (Bytef*)inString.c_str(), inString.size());
  switch (result) {
    case Z_OK:
      compressedSuccess = true;
      break;
    case Z_MEM_ERROR:
      break;
    case Z_BUF_ERROR:
      break;
    default:
      break;
  }

  // we have to base64 the compressed string, otherwise our middleware
  // will not get the JSON right
  outCompressedString = Base64Encode(compressedData, compressedDataSize);

  delete[] compressedData;

  return compressedSuccess;
}
}  // namespace GetGudSdk