/// 24/02/2024
/// assert.cpp

#include <duk_macros/assert.h>
#include <sstream>
#include <stdexcept>

namespace duk::macros {

void duk_assert(bool condition, const char* message, const char* file, int line) {
    if (!condition) {
        std::ostringstream oss;
        oss << "duk_assert: (" << message << ") at \"" << file << "\", line: " << line << std::endl;
        throw std::runtime_error(oss.str());
    }
}

}// namespace duk::macros