#include "Sanitizer.h"

namespace GetgudSDK {
namespace Sanitizer {

	void SanitizeStringChars(std::string& str, char replacementChar) {
		for (char& symbol : str) {
			if (symbol < '!' || symbol > '~' || symbol == '$' || symbol == '\'' ||
				symbol == '\"' || symbol == '%' || symbol == '@' || symbol == '\\') {
				symbol = replacementChar;
			}
		}
	}

	void SanitizeStringCharsSpecial(std::string& str, char replacementChar) {
		for (char& symbol : str) {
			// Note: Allows space (' ') and '@' compared to SanitizeStringChars
			if (symbol < ' ' || symbol > '~' || symbol == '$' || symbol == '\'' ||
				symbol == '\"' || symbol == '%' || symbol == '\\') {
				symbol = replacementChar;
			}
		}
	}

	void SanitizeStringLength(std::string& str, size_t maxLength) {
		if (str.length() > maxLength) {
			if (maxLength >= 3) {
				str.resize(maxLength - 3);
				str += "...";
			} else {
				str.resize(maxLength); // Simple truncation if maxLength is too small for "..."
			}
		}
	}

} // namespace Sanitizer
} // namespace GetgudSDK 