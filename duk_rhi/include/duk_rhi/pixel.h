/// 12/11/2023
/// pixel.h

#ifndef DUK_IMPORT_PIXEL_H
#define DUK_IMPORT_PIXEL_H

#include <duk_hash/hash.h>
#include <duk_macros/macros.h>

#include <array>
#include <cstdint>
#include <cstring>

namespace duk::rhi {

struct PixelFlags {
    enum Bits : uint32_t {
        INTEGER = 1 << 0,
        FLOAT = 1 << 1,
        SIGNED = 1 << 2,
        UNSIGNED = 1 << 3,
        CHANNEL_BIT_WIDTH_8 = 1 << 4,
        CHANNEL_BIT_WIDTH_16 = 1 << 5,
        CHANNEL_BIT_WIDTH_24 = 1 << 6,
        CHANNEL_BIT_WIDTH_32 = 1 << 7,
        CHANNEL_BIT_WIDTH_40 = 1 << 8,
        CHANNEL_LAYOUT_R = 1 << 9,
        CHANNEL_LAYOUT_RG = 1 << 10,
        CHANNEL_LAYOUT_RGB = 1 << 11,
        CHANNEL_LAYOUT_BGR = 1 << 12,
        CHANNEL_LAYOUT_RGBA = 1 << 13,
        CHANNEL_LAYOUT_BGRA = 1 << 14,

        //special for depth formats
        DEPTH = 1 << 15,
        DEPTH_STENCIL = 1 << 16,

        // utils
        INT8 = INTEGER | CHANNEL_BIT_WIDTH_8 | SIGNED,
        INT16 = INTEGER | CHANNEL_BIT_WIDTH_16 | SIGNED,
        INT32 = INTEGER | CHANNEL_BIT_WIDTH_32 | SIGNED,
        UINT8 = INTEGER | CHANNEL_BIT_WIDTH_8 | UNSIGNED,
        UINT16 = INTEGER | CHANNEL_BIT_WIDTH_16 | UNSIGNED,
        UINT32 = INTEGER | CHANNEL_BIT_WIDTH_32 | UNSIGNED,
        FLOAT32 = FLOAT | CHANNEL_BIT_WIDTH_32 | SIGNED,

        DEPTH_MASK = DEPTH | DEPTH_STENCIL,
        CHANNEL_MASK = CHANNEL_LAYOUT_R | CHANNEL_LAYOUT_RG | CHANNEL_LAYOUT_RGB | CHANNEL_LAYOUT_BGR | CHANNEL_LAYOUT_RGBA | CHANNEL_LAYOUT_BGRA,
        TYPE_MASK = INTEGER | FLOAT,
        SIGN_MASK = SIGNED | UNSIGNED,
        BIT_WIDTH_MASK = CHANNEL_BIT_WIDTH_8 | CHANNEL_BIT_WIDTH_16 | CHANNEL_BIT_WIDTH_32,
    };
};

class PixelFormat {
public:
    enum Mask : uint32_t {
        UNDEFINED = 0,
        R8S = PixelFlags::INT8 | PixelFlags::CHANNEL_LAYOUT_R,
        R8U = PixelFlags::UINT8 | PixelFlags::CHANNEL_LAYOUT_R,
        RG8S = PixelFlags::INT8 | PixelFlags::CHANNEL_LAYOUT_RG,
        RG8U = PixelFlags::UINT8 | PixelFlags::CHANNEL_LAYOUT_RG,
        RGB8S = PixelFlags::INT8 | PixelFlags::CHANNEL_LAYOUT_RGB,
        RGB8U = PixelFlags::UINT8 | PixelFlags::CHANNEL_LAYOUT_RGB,
        BGR8S = PixelFlags::INT8 | PixelFlags::CHANNEL_LAYOUT_BGR,
        BGR8U = PixelFlags::UINT8 | PixelFlags::CHANNEL_LAYOUT_BGR,
        RGBA8S = PixelFlags::INT8 | PixelFlags::CHANNEL_LAYOUT_RGBA,
        RGBA8U = PixelFlags::UINT8 | PixelFlags::CHANNEL_LAYOUT_RGBA,
        BGRA8S = PixelFlags::INT8 | PixelFlags::CHANNEL_LAYOUT_BGRA,
        BGRA8U = PixelFlags::UINT8 | PixelFlags::CHANNEL_LAYOUT_BGRA,
        RGBA32S = PixelFlags::INT32 | PixelFlags::CHANNEL_LAYOUT_RGBA,
        RGBA32U = PixelFlags::UINT32 | PixelFlags::CHANNEL_LAYOUT_RGBA,
        RGBA32F = PixelFlags::FLOAT32 | PixelFlags::CHANNEL_LAYOUT_RGBA,
        BGRA32S = PixelFlags::INT32 | PixelFlags::CHANNEL_LAYOUT_BGRA,
        BGRA32U = PixelFlags::UINT32 | PixelFlags::CHANNEL_LAYOUT_BGRA,
        BGRA32F = PixelFlags::FLOAT32 | PixelFlags::CHANNEL_LAYOUT_BGRA,
        D32_SFLOAT = PixelFlags::DEPTH | PixelFlags::CHANNEL_BIT_WIDTH_32 | PixelFlags::CHANNEL_LAYOUT_R,
        D16_UNORM_S8_UINT = PixelFlags::DEPTH_STENCIL | PixelFlags::CHANNEL_BIT_WIDTH_24 | PixelFlags::CHANNEL_LAYOUT_R,
        D24_UNORM_S8_UINT = PixelFlags::DEPTH_STENCIL | PixelFlags::CHANNEL_BIT_WIDTH_32 | PixelFlags::CHANNEL_LAYOUT_R,
        D32_SFLOAT_S8_UINT = PixelFlags::DEPTH_STENCIL | PixelFlags::CHANNEL_BIT_WIDTH_40 | PixelFlags::CHANNEL_LAYOUT_R,
    };

    PixelFormat();

    PixelFormat(uint32_t mask);

    PixelFormat& operator=(uint32_t mask);

    DUK_NO_DISCARD PixelFormat operator&(uint32_t mask) const;

    DUK_NO_DISCARD PixelFormat operator|(uint32_t mask) const;

    DUK_NO_DISCARD operator uint32_t() const;

    DUK_NO_DISCARD uint32_t channel_count() const;

    DUK_NO_DISCARD size_t channel_size() const;

    DUK_NO_DISCARD size_t size() const;

    DUK_NO_DISCARD uint32_t mask() const;

    DUK_NO_DISCARD bool is_depth() const;

    DUK_NO_DISCARD bool is_stencil() const;

private:
    Mask m_mask;
};

uint32_t operator|(uint32_t mask, PixelFormat pixelFormat);

uint32_t operator&(uint32_t mask, PixelFormat pixelFormat);

template<typename ChannelType, uint32_t ChannelCount>
struct Pixel {
    static PixelFormat format() = delete;

    std::array<ChannelType, ChannelCount> channels;
};

using PixelR8S = Pixel<int8_t, 1>;

template<>
inline PixelFormat PixelR8S::format() {
    return PixelFormat::R8S;
}

using PixelR8U = Pixel<uint8_t, 1>;

template<>
inline PixelFormat PixelR8U::format() {
    return PixelFormat::R8U;
}

using PixelRG8S = Pixel<int8_t, 2>;

template<>
inline PixelFormat PixelRG8S::format() {
    return PixelFormat::RG8S;
}

using PixelRG8U = Pixel<uint8_t, 2>;

template<>
inline PixelFormat PixelRG8U::format() {
    return PixelFormat::RG8U;
}

using PixelRGB8S = Pixel<int8_t, 3>;

template<>
inline PixelFormat PixelRGB8S::format() {
    return PixelFormat::RGB8S;
}

using PixelRGB8U = Pixel<uint8_t, 3>;

template<>
inline PixelFormat PixelRGB8U::format() {
    return PixelFormat::RGB8U;
}

using PixelRGBA8S = Pixel<int8_t, 4>;

template<>
inline PixelFormat PixelRGBA8S::format() {
    return PixelFormat::RGBA8S;
}

using PixelRGBA8U = Pixel<uint8_t, 4>;

template<>
inline PixelFormat PixelRGBA8U::format() {
    return PixelFormat::RGBA8U;
}

using PixelRGBA32S = Pixel<int32_t, 4>;

template<>
inline PixelFormat PixelRGBA32S::format() {
    return PixelFormat::RGBA32S;
}

using PixelRGBA32U = Pixel<uint32_t, 4>;

template<>
inline PixelFormat PixelRGBA32U::format() {
    return PixelFormat::RGBA32U;
}

using PixelRGBA32F = Pixel<float, 4>;

template<>
inline PixelFormat PixelRGBA32F::format() {
    return PixelFormat::RGBA32F;
}

}// namespace duk::rhi

namespace std {
template<>
struct hash<duk::rhi::PixelFormat> {
    size_t operator()(const duk::rhi::PixelFormat& pixelFormat) {
        size_t hash = 0;
        duk::hash::hash_combine(hash, pixelFormat.mask());
        return hash;
    }
};

}// namespace std
#endif// DUK_IMPORT_PIXEL_H
