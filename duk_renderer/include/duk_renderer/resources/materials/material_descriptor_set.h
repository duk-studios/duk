/// 17/02/2024
/// material_descriptor_set.h

#ifndef DUK_RENDERER_MATERIAL_DESCRIPTOR_SET_H
#define DUK_RENDERER_MATERIAL_DESCRIPTOR_SET_H

#include <duk_renderer/resources/image.h>
#include <duk_renderer/resources/materials/draw_entry.h>
#include <duk_renderer/resources/materials/instance_buffer.h>
#include <duk_renderer/resources/texture.h>
#include <duk_rhi/command/command_buffer.h>

namespace duk::renderer {

class MaterialDescriptorSet {
public:
    template<typename ResourceT>
    class ResourceIterator {
    public:
        ResourceIterator(MaterialDescriptorSet& descriptorSet, uint32_t i);

        uint32_t index() const;

        ResourceIterator& operator++();

        ResourceT& operator*();

        bool operator==(const ResourceIterator& rhs) const;

        bool operator!=(const ResourceIterator& rhs) const;

    private:
        void next();

    private:
        MaterialDescriptorSet& m_descriptorSet;
        uint32_t m_i;
    };

    template<typename ResourceT>
    class ResourceView {
    public:
        ResourceView(MaterialDescriptorSet& descriptorSet);

        ResourceIterator<ResourceT> begin();

        ResourceIterator<ResourceT> end();

    private:
        MaterialDescriptorSet& m_descriptorSet;
    };

public:
    virtual ~MaterialDescriptorSet() = default;

    DUK_NO_DISCARD ResourceView<ImageResource> images();

    DUK_NO_DISCARD virtual ImageResource& image_at(uint32_t index);

    DUK_NO_DISCARD virtual bool is_image(uint32_t index);

    DUK_NO_DISCARD virtual InstanceBuffer* instance_buffer();

    DUK_NO_DISCARD virtual uint32_t size() const = 0;

    virtual void update(const DrawParams& params) = 0;

    virtual void bind(duk::rhi::CommandBuffer* commandBuffer) = 0;
};

namespace detail {

template<typename ResourceT>
ResourceT& resource_at(MaterialDescriptorSet& descriptorSet, uint32_t index) = delete;

template<>
inline ImageResource& resource_at<ImageResource>(MaterialDescriptorSet& descriptorSet, uint32_t index) {
    return descriptorSet.image_at(index);
}

template<typename ResourceT>
bool is_resource(MaterialDescriptorSet& descriptorSet, uint32_t index) = delete;

template<>
inline bool is_resource<ImageResource>(MaterialDescriptorSet& descriptorSet, uint32_t index) {
    return descriptorSet.is_image(index);
}

}// namespace detail

template<typename ResourceT>
ResourceT& MaterialDescriptorSet::ResourceIterator<ResourceT>::operator*() {
    return detail::resource_at<ResourceT>(m_descriptorSet, m_i);
}

template<typename ResourceT>
MaterialDescriptorSet::ResourceIterator<ResourceT>::ResourceIterator(MaterialDescriptorSet& descriptorSet, uint32_t i)
    : m_descriptorSet(descriptorSet)
    , m_i(i) {
    next();
}

template<typename ResourceT>
uint32_t MaterialDescriptorSet::ResourceIterator<ResourceT>::index() const {
    return m_i;
}

template<typename ResourceT>
MaterialDescriptorSet::ResourceIterator<ResourceT>& MaterialDescriptorSet::ResourceIterator<ResourceT>::operator++() {
    ++m_i;
    next();
    return *this;
}

template<typename ResourceT>
bool MaterialDescriptorSet::ResourceIterator<ResourceT>::operator==(const MaterialDescriptorSet::ResourceIterator<ResourceT>& rhs) const {
    return &m_descriptorSet == &rhs.m_descriptorSet && m_i == rhs.m_i;
}

template<typename ResourceT>
bool MaterialDescriptorSet::ResourceIterator<ResourceT>::operator!=(const MaterialDescriptorSet::ResourceIterator<ResourceT>& rhs) const {
    return &m_descriptorSet != &rhs.m_descriptorSet || m_i != rhs.m_i;
}

template<typename ResourceT>
void MaterialDescriptorSet::ResourceIterator<ResourceT>::next() {
    while (m_i < m_descriptorSet.size() && !detail::is_resource<ResourceT>(m_descriptorSet, m_i)) {
        ++m_i;
    }
}

template<typename ResourceT>
MaterialDescriptorSet::ResourceView<ResourceT>::ResourceView(MaterialDescriptorSet& descriptorSet)
    : m_descriptorSet(descriptorSet) {
}

template<typename ResourceT>
MaterialDescriptorSet::ResourceIterator<ResourceT> MaterialDescriptorSet::ResourceView<ResourceT>::begin() {
    return MaterialDescriptorSet::ResourceIterator<ResourceT>(m_descriptorSet, 0);
}

template<typename ResourceT>
MaterialDescriptorSet::ResourceIterator<ResourceT> MaterialDescriptorSet::ResourceView<ResourceT>::end() {
    return MaterialDescriptorSet::ResourceIterator<ResourceT>(m_descriptorSet, m_descriptorSet.size());
}

}// namespace duk::renderer

#endif// DUK_RENDERER_MATERIAL_DESCRIPTOR_SET_H
