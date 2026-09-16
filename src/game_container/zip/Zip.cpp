#include "Zip.h"
#include "../../utils/Utils.h"

namespace GetgudSDK {
	Zipper zipper;

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
}  // namespace GetgudSDK