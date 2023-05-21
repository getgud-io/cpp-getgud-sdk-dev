#include "../utils/Validator.h"
#include "../../include/actions/Helpers.h"
#include "../config/Config.h"

namespace GetGudSdk {
extern Config sdkConfig;

namespace Validator {

/**
 * ValidateItemValue:
 *
 * Validate that item value is within allowed range
 **/
bool ValidateItemValue(int sdkValue, int minAllowedValue, int maxAllowedValue) {
  if (sdkValue < minAllowedValue || sdkValue > maxAllowedValue)
    return false;

  return true;
}

/**
 * ValidateItemValue:
 *
 * Validate that item value is within allowed range
 **/
bool ValidateItemValue(unsigned int sdkValue,
                       unsigned int minAllowedValue,
                       unsigned int maxAllowedValue) {
  if (sdkValue < minAllowedValue || sdkValue > maxAllowedValue)
    return false;

  return true;
}

/**
 * ValidateItemValue:
 *
 * Validate that item value is within allowed range
 **/
bool ValidateItemValue(float sdkValue,
                       float minAllowedValue,
                       float maxAllowedValue) {
  if (sdkValue < minAllowedValue || sdkValue > maxAllowedValue)
    return false;

  return true;
}

/**
 * ValidateItemValue:
 *
 * Validate that item value is within allowed range
 **/
bool ValidateItemValue(long long sdkValue,
                       long long minAllowedValue,
                       long long maxAllowedValue) {
  if (sdkValue < minAllowedValue || sdkValue > maxAllowedValue)
    return false;

  return true;
}

/**
 * ValidateStringLength:
 *
 * Validate that string length is within allowed range
 **/
bool ValidateStringLength(std::string& sdkString,
                          int minAllowedLength,
                          int maxAllowedLength) {
  if (sdkString.size() < minAllowedLength ||
      sdkString.size() > maxAllowedLength)
    return false;

  return true;
}

/**
 * ValidateStringChars:
 *
 * Validate that we are using only allowed chars in the string
 **/
bool ValidateStringChars(std::string& sdkString) {
  for (auto& symbol : sdkString) {
    if (symbol < '!' && symbol > '~')
      return false;
    if (symbol == '$' || symbol == '\'' || symbol == '\"' || symbol == '%' ||
        symbol == '@')
      return false;
  }
  return true;
}

}  // namespace Validator
}  // namespace GetGudSdk