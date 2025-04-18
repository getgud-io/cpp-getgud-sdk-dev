#pragma once

#include <string>

namespace GetgudSDK {
namespace Sanitizer {

	/**
	 * SanitizeStringChars:
	 *
	 * Replaces disallowed characters in a string with a specified replacement character.
	 * Disallowed characters are those outside the ASCII range '!' to '~', or specific symbols like '$', ''', '"', '%', '@'.
	 */
	void SanitizeStringChars(std::string& str, char replacementChar = '_');

	/**
	 * SanitizeStringCharsSpecial:
	 *
	 * Replaces disallowed characters in a string with a specified replacement character, using a more permissive ruleset.
	 * Disallowed characters are those outside the ASCII range ' ' to '~', or specific symbols like '$', ''', '"', '%'. Allows '@'.
	 */
	void SanitizeStringCharsSpecial(std::string& str, char replacementChar = '_');

	/**
	 * SanitizeStringLength:
	 *
	 * Ensures a string does not exceed a maximum length. If the string is longer,
	 * it truncates it and appends "..." if the maxLength is 3 or greater.
	 */
	void SanitizeStringLength(std::string& str, size_t maxLength);

} // namespace Sanitizer
} // namespace GetgudSDK 