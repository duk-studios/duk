/// 04/06/2023
/// image_data_source.h

#ifndef DUK_RHI_IMAGE_DATA_SOURCE_H
#define DUK_RHI_IMAGE_DATA_SOURCE_H

#include <duk_hash/data_source.h>
#include <duk_rhi/image.h>

#include <cstring>
#include <stdexcept>

namespace duk::rhi {

class ImageDataSource : public duk::hash::DataSource {
public:

    DUK_NO_DISCARD virtual PixelFormat pixel_format() const = 0;

    DUK_NO_DISCARD virtual size_t pixel_count() const = 0;

    DUK_NO_DISCARD virtual size_t byte_count() const = 0;

    DUK_NO_DISCARD virtual uint32_t width() const = 0;

    DUK_NO_DISCARD virtual uint32_t height() const = 0;

    DUK_NO_DISCARD virtual bool has_data() const = 0;

    virtual void read_bytes(void* dst, size_t size, size_t offset) const = 0;

    template<typename PixelType>
    void read_pixels(PixelType* dst, size_t pixelCount, size_t pixelOffset) {
        const auto format = pixel_format();
        const auto pixelByteSize = format;
        assert(pixelByteSize == sizeof(PixelType));
        assert(pixel_count() >= pixelCount + pixelOffset);
        read_bytes((void*)dst, pixelCount * pixelByteSize, pixelOffset * pixelByteSize);
    }
};

template<typename PixelType>
class ImageDataSourceT : public ImageDataSource {
public:
    ImageDataSourceT(uint32_t width, uint32_t height) :
        m_pixels(width * height),
        m_width(width),
        m_height(height) {
        assert(width * height == m_pixels.size());
    }

    ImageDataSourceT(std::vector<PixelType>&& pixels, uint32_t width, uint32_t height) :
        m_pixels(std::move(pixels)),
        m_width(width),
        m_height(height) {
        assert(width * height == m_pixels.size());
    }

    ImageDataSourceT(const std::vector<PixelType>& pixels, uint32_t width, uint32_t height) :
        m_pixels(pixels),
        m_width(width),
        m_height(height) {
        assert(width * height == m_pixels.size());
    }

    ImageDataSourceT(const PixelType* pixels, uint32_t width, uint32_t height) :
        m_pixels(pixels, pixels + (width * height)),
        m_width(width),
        m_height(height) {
        assert(width * height == m_pixels.size());
    }

    DUK_NO_DISCARD PixelType* data() {
        return m_pixels.data();
    }

    DUK_NO_DISCARD const PixelType* data() const {
        return m_pixels.data();
    }

    DUK_NO_DISCARD PixelType& operator[](size_t index) {
        return m_pixels[index];
    }

    DUK_NO_DISCARD const PixelType& operator[](size_t index) const {
        return m_pixels[index];
    }

    DUK_NO_DISCARD PixelType& at(uint32_t column, uint32_t row) {
        return m_pixels[column + (row * m_width)];
    }

    DUK_NO_DISCARD const PixelType& at(uint32_t column, uint32_t row) const {
        return m_pixels[column + (row * m_width)];
    }

    DUK_NO_DISCARD PixelFormat pixel_format() const override {
        return PixelType::format();
    }

    DUK_NO_DISCARD size_t pixel_count() const override {
        return m_pixels.size();
    }

    DUK_NO_DISCARD size_t byte_count() const override {
        return m_pixels.size() * sizeof(PixelType);
    }

    DUK_NO_DISCARD uint32_t width() const override {
        return m_width;
    }

    DUK_NO_DISCARD uint32_t height() const override {
        return m_height;
    }

    DUK_NO_DISCARD bool has_data() const override {
        return !m_pixels.empty();
    }

    void read_bytes(void* dst, size_t size, size_t offset) const override {
        assert(byte_count() >= size + offset);
        const uint8_t* src = (const uint8_t*)m_pixels.data();
        memcpy(dst, (const void*)(src + offset), size);
    }

protected:

    DUK_NO_DISCARD duk::hash::Hash calculate_hash() const override {
        duk::hash::Hash hash = 0;
        duk::hash::hash_combine(hash, m_pixels.data(), byte_count());
        duk::hash::hash_combine(hash, m_width);
        duk::hash::hash_combine(hash, m_height);
        duk::hash::hash_combine(hash, PixelType::format());
        return hash;
    }

private:
    std::vector<PixelType> m_pixels;
    uint32_t m_width;
    uint32_t m_height;
};

using ImageDataSourceR8S = ImageDataSourceT<duk::rhi::PixelR8S>;
using ImageDataSourceR8U = ImageDataSourceT<duk::rhi::PixelR8U>;
using ImageDataSourceRG8S = ImageDataSourceT<duk::rhi::PixelRG8S>;
using ImageDataSourceRG8U = ImageDataSourceT<duk::rhi::PixelRG8U>;
using ImageDataSourceRGBA8S = ImageDataSourceT<duk::rhi::PixelRGBA8S>;
using ImageDataSourceRGBA8U = ImageDataSourceT<duk::rhi::PixelRGBA8U>;
using ImageDataSourceRGBA32S = ImageDataSourceT<duk::rhi::PixelRGBA32S>;
using ImageDataSourceRGBA32U = ImageDataSourceT<duk::rhi::PixelRGBA32U>;
using ImageDataSourceRGBA32F = ImageDataSourceT<duk::rhi::PixelRGBA32F>;

class ImageDataSourceEmpty : public ImageDataSource {
public:
    ImageDataSourceEmpty(uint32_t width, uint32_t height, PixelFormat pixelFormat) :
        m_width(width),
        m_height(height),
        m_pixelFormat(pixelFormat) {

    }

    DUK_NO_DISCARD PixelFormat pixel_format() const override {
        return m_pixelFormat;
    }

    DUK_NO_DISCARD size_t pixel_count() const override {
        return m_width * m_height;
    }

    DUK_NO_DISCARD size_t byte_count() const override {
        return pixel_count() * m_pixelFormat.size();
    }

    DUK_NO_DISCARD uint32_t width() const override {
        return m_width;
    }

    DUK_NO_DISCARD uint32_t height() const override {
        return m_height;
    }

    DUK_NO_DISCARD bool has_data() const override {
        return false;
    }

    void read_bytes(void* dst, size_t size, size_t offset) const override {
        throw std::logic_error("tried to read from an ImageDataSourceEmpty");
    }

protected:

    DUK_NO_DISCARD duk::hash::Hash calculate_hash() const override {
        duk::hash::Hash hash = 0;
        duk::hash::hash_combine(hash, m_width);
        duk::hash::hash_combine(hash, m_height);
        duk::hash::hash_combine(hash, m_pixelFormat);
        return hash;
    }

private:
    uint32_t m_width;
    uint32_t m_height;
    PixelFormat m_pixelFormat;
};

}

#endif // DUK_RHI_IMAGE_DATA_SOURCE_H

