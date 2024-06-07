#include <zconf.h>
#include <zlib.h>
#include <string>

namespace GetgudSDK {
class Zipper {
 public:
  bool CompressString(std::string& inString,
                      std::string& outCompressedString);
};
}  // namespace GetgudSDK