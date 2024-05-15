#include <zlib/zconf.h>
#include <zlib/zlib.h>
#include <string>

namespace GetgudSDK {
class Zipper {
 public:
  bool CompressString(std::string& inString,
                      std::string& outCompressedString);
};
}  // namespace GetgudSDK