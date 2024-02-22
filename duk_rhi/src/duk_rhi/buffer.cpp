/// 28/05/2023
/// buffer.cpp

#include <duk_rhi/buffer.h>

namespace duk::rhi {

Buffer::~Buffer() = default;

const uint8_t* Buffer::read_ptr() const {
    return read_ptr(0);
}

uint8_t* Buffer::write_ptr() {
    return write_ptr(0);
}

}// namespace duk::rhi