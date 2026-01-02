/// 23/04/2023
/// sampler.cpp

#include <duk_rhi/sampler.h>

namespace duk::rhi {

bool operator==(const Sampler& lhs, const Sampler& rhs) noexcept {
    return lhs.wrapMode == rhs.wrapMode && lhs.filter == rhs.filter;
}

}// namespace duk::rhi

namespace duk::serial {

namespace detail {

static duk::rhi::Sampler::WrapMode parse_wrap_mode(const char* str) {
    if (strcmp(str, "repeat") == 0) {
        return rhi::Sampler::WrapMode::REPEAT;
    }
    if (strcmp(str, "mirror-repeat") == 0) {
        return rhi::Sampler::WrapMode::MIRRORED_REPEAT;
    }
    if (strcmp(str, "clamp-edge") == 0) {
        return rhi::Sampler::WrapMode::CLAMP_TO_EDGE;
    }
    if (strcmp(str, "clamp-border") == 0) {
        return rhi::Sampler::WrapMode::CLAMP_TO_BORDER;
    }
    if (strcmp(str, "mirror-clamp-edge") == 0) {
        return rhi::Sampler::WrapMode::MIRROR_CLAMP_TO_EDGE;
    }
    throw std::invalid_argument("unknown wrap mode name");
}

static duk::rhi::Sampler::Filter parse_filter(const char* str) {
    if (strcmp(str, "nearest") == 0) {
        return rhi::Sampler::Filter::NEAREST;
    }
    if (strcmp(str, "linear") == 0) {
        return rhi::Sampler::Filter::LINEAR;
    }
    if (strcmp(str, "cubic") == 0) {
        return rhi::Sampler::Filter::CUBIC;
    }
    throw std::invalid_argument("unknown filter name");
}

static std::string to_string(const duk::rhi::Sampler::WrapMode& wrapMode) {
    switch (wrapMode) {
        case rhi::Sampler::WrapMode::REPEAT:
            return "repeat";
        case rhi::Sampler::WrapMode::MIRRORED_REPEAT:
            return "mirror-repeat";
        case rhi::Sampler::WrapMode::CLAMP_TO_EDGE:
            return "clamp-edge";
        case rhi::Sampler::WrapMode::CLAMP_TO_BORDER:
            return "clamp-border";
        case rhi::Sampler::WrapMode::MIRROR_CLAMP_TO_EDGE:
            return "mirror-clamp-edge";
    }
    throw std::invalid_argument("unknown duk::rhi::Sampler::WrapMode");
}

static std::string to_string(const duk::rhi::Sampler::Filter& filter) {
    switch (filter) {
        case rhi::Sampler::Filter::NEAREST:
            return "nearest";
        case rhi::Sampler::Filter::LINEAR:
            return "linear";
        case rhi::Sampler::Filter::CUBIC:
            return "cubic";
    }
    throw std::invalid_argument("unknown duk::rhi::Sampler::Filter");
}

}// namespace detail


void JsonPrimitiveValue<rhi::Sampler::WrapMode>::write(rapidjson::Document& document, rapidjson::Value& json, const rhi::Sampler::WrapMode& value) {
    json.SetString(detail::to_string(value), document.GetAllocator());
}

void JsonPrimitiveValue<rhi::Sampler::WrapMode>::read(const rapidjson::Value& json, rhi::Sampler::WrapMode& value) {
    value = detail::parse_wrap_mode(json.GetString());
}

void JsonPrimitiveValue<rhi::Sampler::Filter>::write(rapidjson::Document& document, rapidjson::Value& json, const rhi::Sampler::Filter& value) {
    json.SetString(detail::to_string(value), document.GetAllocator());
}

void JsonPrimitiveValue<rhi::Sampler::Filter>::read(const rapidjson::Value& json, rhi::Sampler::Filter& value) {
    value = detail::parse_filter(json.GetString());
}
}
