#include <zlib/zconf.h>
#include <zlib/zlib.h>
#include <string>

namespace GetGudSdk {
class Zipper {
 public:
  bool CompressString(std::string& inString,
                      std::string& outCompressedString);
};
}  // namespace GetGudSdk