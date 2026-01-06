/// 23/04/2023
/// sampler.cpp

#include <duk_rhi/sampler.h>

namespace duk::rhi {

bool operator==(const Sampler& lhs, const Sampler& rhs) noexcept {
    return lhs.wrapMode == rhs.wrapMode && lhs.filter == rhs.filter;
}

}// namespace duk::rhi