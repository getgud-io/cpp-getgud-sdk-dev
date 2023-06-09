#include <string>

namespace GetGudSdk {
namespace Validator {
bool ValidateItemValue(int sdkValue, int minAllowedValue, int maxAllowedValue);
bool ValidateItemValue(unsigned int sdkValue,
                       unsigned int minAllowedValue,
                       unsigned int maxAllowedValue);
bool ValidateItemValue(float sdkValue,
                       float minAllowedValue,
                       float maxAllowedValue);
bool ValidateItemValue(long long sdkValue,
                       long long minAllowedValue,
                       long long maxAllowedValue);
bool ValidateStringLength(std::string& sdkString,
                          int minAllowedLength,
                          int maxAllowedLength);
bool ValidateStringChars(std::string& sdkString);
bool ValidateStringCharsSpecial(std::string& sdkString);
bool ValidateGuid(std::string& guid);
bool ValidateActionType(unsigned int actionType);
}  // namespace Validator
}  // namespace GetGudSdk