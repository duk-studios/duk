/// 23/04/2023
/// image.cpp

#include <duk_rhi/image.h>

namespace duk::rhi {

Image::~Image() = default;

glm::uvec2 Image::size() const {
    return {width(), height()};
}

}
