/// 05/10/2023
/// file.h

#ifndef DUK_TOOLS_FILE_H
#define DUK_TOOLS_FILE_H

#include <cstdint>
#include <string>
#include <vector>

namespace duk::tools {

class File {
public:
    static std::vector<uint8_t> load_bytes(const char* filepath);

    static std::string load_text(const char* filepath);
};

}// namespace duk::tools

#endif// DUK_TOOLS_FILE_H
