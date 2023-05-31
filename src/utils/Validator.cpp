#include "pch.h"
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

/**
 * ValidateGuid:
 *
 * Validate that we are using only allowed GUID by ISO/IEC
 **/
bool ValidateGuid(std::string& guid) {

  for (auto& symbol : guid) {
    if (symbol < '0' && symbol > '9' && symbol < 'a' && symbol > 'z' &&
        symbol < 'A' && symbol > 'Z' && symbol != '-')
      return false;
  }

  return true;
}

/**
 * ValidateActionType:
 *
 * Validate that we are using only allowed actions
 * Actions type can be created by int casting into Actions type
 **/
bool ValidateActionType(unsigned int actionType) {
  if (actionType > 0U &&
      actionType <= (unsigned int)Actions::Spawn)
    return true;
  else
    return false;
}

}  // namespace Validator
}  // namespace GetGudSdk