/// 23/04/2023
/// sampler.cpp

#include <duk_rhi/sampler.h>

namespace duk::rhi {

bool Sampler::operator==(const Sampler& rhs) const noexcept {
    return wrapMode == rhs.wrapMode && filter == rhs.filter;
}

}// namespace duk::rhi