/// 05/10/2023
/// pass.cpp

#include <duk_renderer/passes/pass.h>

#include <algorithm>
#include <cassert>

namespace duk::renderer {

PassConnection::PassConnection(duk::rhi::Access::Mask accessMask, duk::rhi::PipelineStage::Mask stageMask, duk::rhi::Image::Layout imageLayout) :
    m_accessMask(accessMask),
    m_stageMask(stageMask),
    m_imageLayout(imageLayout),
    m_image(nullptr),
    m_parent(nullptr) {

}

PassConnection::~PassConnection() {
    for (auto child : m_children) {
        child->m_parent = nullptr;
    }
}

void PassConnection::update(duk::rhi::Image* image) {
    m_image = image;
    for (auto& child : m_children) {
        child->update(image);
    }
}

void PassConnection::connect(PassConnection* connection) {
    auto previousParent = connection->m_parent;
    if (previousParent) {
        // we need to disconnect the previous connection
        previousParent->disconnect(connection);
    }

    m_children.push_back(connection);
    connection->m_parent = this;
}

void PassConnection::disconnect(PassConnection* connection) {
    assert(connection->m_parent == this);
    auto it = std::find(m_children.begin(), m_children.end(), connection);
    m_children.erase(it);
    connection->m_parent = nullptr;
}

duk::rhi::Image* PassConnection::image() const {
    return m_image;
}

duk::rhi::Access::Mask PassConnection::access_mask() const {
    return m_accessMask;
}

duk::rhi::PipelineStage::Mask PassConnection::stage_mask() const {
    return m_stageMask;
}

duk::rhi::Image::Layout PassConnection::image_layout() const {
    return m_imageLayout;
}

duk::rhi::Access::Mask PassConnection::parent_access_mask() const {
    if (m_parent) {
        return m_parent->access_mask();
    }
    return duk::rhi::Access::NONE;
}

duk::rhi::PipelineStage::Mask PassConnection::parent_stage_mask() const {
    if (m_parent) {
        return m_parent->stage_mask();
    }
    return duk::rhi::PipelineStage::NONE;
}

duk::rhi::Image::Layout PassConnection::parent_image_layout() const {
    if (m_parent) {
        return m_parent->image_layout();
    }
    return rhi::Image::Layout::UNDEFINED;
}

duk::rhi::CommandBuffer::ImageMemoryBarrier PassConnection::image_memory_barrier() const {
    duk::rhi::CommandBuffer::ImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.image = m_image;
    imageMemoryBarrier.srcStageMask = parent_stage_mask();
    imageMemoryBarrier.dstStageMask = stage_mask();
    imageMemoryBarrier.srcAccessMask = parent_access_mask();
    imageMemoryBarrier.dstAccessMask = access_mask();
    imageMemoryBarrier.oldLayout = parent_image_layout();
    imageMemoryBarrier.newLayout = image_layout();
    imageMemoryBarrier.subresourceRange = duk::rhi::Image::kFullSubresourceRange;
    return imageMemoryBarrier;
}

Pass::~Pass() = default;

}