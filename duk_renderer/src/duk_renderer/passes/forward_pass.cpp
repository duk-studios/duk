/// 05/10/2023
/// forward_pass.cpp

#include <duk_renderer/components/material_slot.h>
#include <duk_renderer/components/mesh_slot.h>
#include <duk_renderer/material/material.h>
#include <duk_renderer/mesh/mesh.h>
#include <duk_renderer/passes/forward_pass.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/material/globals/global_descriptors.h>
#include <duk_renderer/mesh/draw_buffer.h>
#include <duk_renderer/mesh/mesh_buffer.h>

#include "duk_rhi/image_data_source.h"

namespace duk::renderer {

namespace detail {

static constexpr auto kColorFormat = duk::rhi::PixelFormat::RGBA8U;

static std::shared_ptr<duk::rhi::Image> create_color_image(duk::rhi::RHI* rhi, duk::rhi::CommandQueue* commandQueue, uint32_t width, uint32_t height) {
    duk::rhi::ImageDataSourceEmpty colorImageDataSource(width, height, kColorFormat);
    colorImageDataSource.update_hash();

    duk::rhi::RHI::ImageCreateInfo colorImageCreateInfo = {};
    colorImageCreateInfo.usage = duk::rhi::Image::Usage::COLOR_ATTACHMENT;
    colorImageCreateInfo.initialLayout = duk::rhi::Image::Layout::COLOR_ATTACHMENT;
    colorImageCreateInfo.updateFrequency = duk::rhi::Image::UpdateFrequency::DEVICE_DYNAMIC;
    colorImageCreateInfo.imageDataSource = &colorImageDataSource;
    colorImageCreateInfo.commandQueue = commandQueue;
    colorImageCreateInfo.dstStages = duk::rhi::PipelineStage::FRAGMENT_SHADER | duk::rhi::PipelineStage::COLOR_ATTACHMENT_OUTPUT;

    auto colorImage = rhi->create_image(colorImageCreateInfo);

    if (!colorImage) {
        throw std::runtime_error("failed to create color image!");
    }

    return colorImage;
}

static std::shared_ptr<duk::rhi::Image> create_depth_image(duk::rhi::RHI* rhi, duk::rhi::CommandQueue* commandQueue, uint32_t width, uint32_t height) {
    duk::rhi::ImageDataSourceEmpty depthImageDataSource(width, height, rhi->capabilities()->depth_format());
    depthImageDataSource.update_hash();

    duk::rhi::RHI::ImageCreateInfo depthImageCreateInfo = {};
    depthImageCreateInfo.usage = duk::rhi::Image::Usage::DEPTH_STENCIL_ATTACHMENT;
    depthImageCreateInfo.initialLayout = duk::rhi::Image::Layout::DEPTH_STENCIL_ATTACHMENT;
    depthImageCreateInfo.updateFrequency = duk::rhi::Image::UpdateFrequency::DEVICE_DYNAMIC;
    depthImageCreateInfo.imageDataSource = &depthImageDataSource;
    depthImageCreateInfo.commandQueue = commandQueue;
    depthImageCreateInfo.dstStages = duk::rhi::PipelineStage::EARLY_FRAGMENT_TESTS;

    auto depthImage = rhi->create_image(depthImageCreateInfo);

    if (!depthImage) {
        throw std::runtime_error("failed to create depth image!");
    }

    return depthImage;
}

static SortKey calculate_opaque_sort_key(const duk::objects::Object& object) {
    auto [meshSlot, materialSlot] = object.components<MeshSlot, MaterialSlot>();
    SortKey sortKey = {};
    sortKey.bytes16x4.chunks[0] = reinterpret_cast<std::intptr_t>(meshSlot->mesh.get());
    sortKey.bytes16x4.chunks[1] = reinterpret_cast<std::intptr_t>(materialSlot->material.get());
    sortKey.bytes32x2.chunks[1] = materialSlot->material->shader()->priority();
    return sortKey;
}

static uint32_t float_to_uint32(float value) {
    const float rangeFloat = 2000.0f;
    const float halfRange = rangeFloat * 0.5f;

    value = std::clamp(value, -halfRange, halfRange);

    // Calculate the maximum value of an unsigned int
    const uint32_t maxUInt = 30000;

    // Apply the linear transformation
    uint32_t result = static_cast<uint32_t>((value + halfRange) / rangeFloat * maxUInt);

    return result;
}

static uint32_t calculate_z_order(const duk::objects::Object& object) {
    const auto transform = object.component<Transform>();
    if (!transform) {
        return 0;
    }
    // TODO: this should be relative to the camera
    return float_to_uint32(transform->position.z);
}

static SortKey calculate_transparent_sort_key(const duk::objects::Object& object) {
    auto [meshSlot, materialSlot] = object.components<MeshSlot, MaterialSlot>();
    SortKey sortKey = {};
    sortKey.bytes16x4.chunks[0] = reinterpret_cast<std::intptr_t>(meshSlot->mesh.get());
    sortKey.bytes16x4.chunks[1] = reinterpret_cast<std::intptr_t>(materialSlot->material.get());
    sortKey.bytes16x4.chunks[2] = static_cast<uint16_t>(calculate_z_order(object));
    sortKey.bytes16x4.chunks[3] = materialSlot->material->shader()->priority();

    return sortKey;
}

static bool is_transparent(const Material* material) {
    return material->shader()->blend();
}

static bool compatible(const ObjectEntry& meshEntry, const DrawEntry& drawEntry) {
    return meshEntry.mesh == drawEntry.mesh && meshEntry.material == drawEntry.material;
}

static bool valid(const DrawEntry& drawEntry) {
    return drawEntry.material != nullptr && drawEntry.mesh != nullptr && drawEntry.instanceCount > 0;
}

static void update_instance_draw_entires(DrawGroupData& drawGroup) {
    DrawEntry drawEntry = {};

    for (const auto sortedIndex: drawGroup.sortedIndices) {
        auto& objectEntry = drawGroup.objectEntries[sortedIndex];

        const auto material = objectEntry.material;

        // update instance buffers
        material->push(objectEntry.objectId);

        if (compatible(objectEntry, drawEntry)) {
            drawEntry.instanceCount++;
            continue;
        }

        if (valid(drawEntry)) {
            drawGroup.instanceDrawEntries.push_back(drawEntry);
        }

        // if the material is different, it's an entire new draw entry starting at 0
        if (drawEntry.material != material) {
            drawEntry.instanceCount = 1;
            // minus one to count for this exact instance
            drawEntry.firstInstance = material->instance_count() - 1;
        }
        // if only the mesh is different, just jump to the correct instance start
        else if (drawEntry.mesh != objectEntry.mesh) {
            drawEntry.firstInstance += drawEntry.instanceCount;
            drawEntry.instanceCount = 1;
        }
        drawEntry.material = material;
        drawEntry.mesh = objectEntry.mesh;
    }
    if (valid(drawEntry)) {
        drawGroup.instanceDrawEntries.push_back(drawEntry);
    }
}

static bool compatible(const IndirectDrawEntry& indirectDrawEntry, const DrawEntry& drawEntry) {
    return indirectDrawEntry.meshBuffer == drawEntry.mesh->buffer() && indirectDrawEntry.material == drawEntry.material;
}

static bool valid(const IndirectDrawEntry& indirectDrawEntry) {
    return indirectDrawEntry.material != nullptr && indirectDrawEntry.drawCount > 0;
}

static void update_indirect_draw_entires(DrawGroupData& drawGroup, DrawBuffer* drawBuffer) {
    IndirectDrawEntry indirectDrawEntry = {};
    for (auto& instanceDrawEntry: drawGroup.instanceDrawEntries) {
        const auto material = instanceDrawEntry.material;
        const auto mesh = instanceDrawEntry.mesh;
        const auto meshBuffer = mesh->buffer();
        const auto offset = drawBuffer->size();

        if (meshBuffer == nullptr || meshBuffer->index_type() == duk::rhi::IndexType::NONE) {
            drawBuffer->push_draw(mesh->vertex_count(), instanceDrawEntry.instanceCount, mesh->vertex_offset(), instanceDrawEntry.firstInstance);
        } else {
            drawBuffer->push_indexed_draw(mesh->index_count(), instanceDrawEntry.instanceCount, mesh->index_offset(), mesh->vertex_offset(), instanceDrawEntry.firstInstance);
        }

        if (compatible(indirectDrawEntry, instanceDrawEntry)) {
            indirectDrawEntry.drawCount++;
            continue;
        }

        if (valid(indirectDrawEntry)) {
            drawGroup.drawEntries.push_back(indirectDrawEntry);
        }

        indirectDrawEntry.drawCount = 1;
        indirectDrawEntry.material = material;
        indirectDrawEntry.meshBuffer = meshBuffer;
        indirectDrawEntry.offset = offset;
    }
    if (valid(indirectDrawEntry)) {
        drawGroup.drawEntries.push_back(indirectDrawEntry);
    }
}

static void update_draw_group(DrawGroupData& drawGroup, DrawBuffer* drawBuffer) {
    SortKey::sort_indices(drawGroup.objectEntries, drawGroup.sortedIndices);

    update_instance_draw_entires(drawGroup);

    update_indirect_draw_entires(drawGroup, drawBuffer);
}

static void update_draw_data(const Pass::UpdateParams& params, duk::rhi::RenderPass* renderPass, DrawData& drawData) {
    drawData.clear();
    const auto objects = params.objects;
    auto& opaqueGroup = drawData.opaqueGroup;
    auto& transparentGroup = drawData.transparentGroup;

    std::set<Material*> uniqueMaterials;

    for (auto object: objects->all_with<MeshSlot, MaterialSlot>()) {
        auto meshSlot = object.component<MeshSlot>();
        auto materialSlot = object.component<MaterialSlot>();

        if (!meshSlot->mesh) {
            duk::log::info("MeshSlot with no Mesh, skipping...");
            continue;
        }

        if (!materialSlot->material) {
            duk::log::info("MaterialSlot with no Material, skipping...");
            continue;
        }

        auto material = materialSlot->material.get();
        ObjectEntry* objectEntry;
        DrawGroupData::CalculateSortKeyFunc calculateSortKey;
        if (is_transparent(material)) {
            objectEntry = &transparentGroup.objectEntries.emplace_back();
            calculateSortKey = transparentGroup.calculateSortKey;
        } else {
            objectEntry = &opaqueGroup.objectEntries.emplace_back();
            calculateSortKey = opaqueGroup.calculateSortKey;
        }

        objectEntry->objectId = object.id();
        objectEntry->mesh = meshSlot->mesh.get();
        objectEntry->material = material;
        objectEntry->sortKey = calculateSortKey(object);

        uniqueMaterials.insert(material);
    }

    if (opaqueGroup.objectEntries.empty() && transparentGroup.objectEntries.empty()) {
        return;
    }

    for (const auto material: uniqueMaterials) {
        MaterialLocationId cameraLocationId = material->find_binding("uCamera");
        if (cameraLocationId.valid()) {
            auto cameraDescriptor = material->get(cameraLocationId);
            if (cameraDescriptor.type() == duk::rhi::DescriptorType::UNDEFINED) {
                material->set(cameraLocationId, params.globalDescriptors->camera_ubo()->descriptor());
            }
        }
        MaterialLocationId lights = material->find_binding("uLights");
        if (lights.valid()) {
            material->set(lights, params.globalDescriptors->lights_ubo()->descriptor());
        }
        material->clear();
    }

    update_draw_group(drawData.opaqueGroup, drawData.drawBuffer.get());
    update_draw_group(drawData.transparentGroup, drawData.drawBuffer.get());

    drawData.drawBuffer->flush();

    for (const auto material: uniqueMaterials) {
        material->update(*params.pipelineCache, renderPass, params.viewport);
    }
}

static void render_draw_indirect(duk::rhi::CommandBuffer* commandBuffer, const DrawBuffer* drawBuffer, const IndirectDrawEntry& entry, bool multiDrawSupported) {
    if (multiDrawSupported) {
        commandBuffer->draw_indirect(drawBuffer->buffer(), entry.offset, entry.drawCount);
    } else {
        for (auto i = 0; i < entry.drawCount; i++) {
            const auto offset = entry.offset + i * sizeof(IndirectDrawCommand);
            commandBuffer->draw_indirect(drawBuffer->buffer(), offset, 1);
        }
    }
}

static void render_draw_indexed_indirect(duk::rhi::CommandBuffer* commandBuffer, const DrawBuffer* drawBuffer, const IndirectDrawEntry& entry, bool multiDrawSupported) {
    if (multiDrawSupported) {
        commandBuffer->draw_indirect_indexed(drawBuffer->buffer(), entry.offset, entry.drawCount);
    } else {
        for (auto i = 0; i < entry.drawCount; i++) {
            const auto offset = entry.offset + i * sizeof(IndirectDrawIndexedCommand);
            commandBuffer->draw_indirect_indexed(drawBuffer->buffer(), offset, 1);
        }
    }
}

static void render_draw_group(duk::rhi::CommandBuffer* commandBuffer, const DrawBuffer* drawBuffer, const DrawGroupData& drawGroup, bool multiDrawSupported) {
    const Material* currentMaterial = nullptr;
    const MeshBuffer* currentMeshBuffer = nullptr;
    for (auto& entry: drawGroup.drawEntries) {
        if (currentMaterial != entry.material) {
            currentMaterial = entry.material;
            currentMaterial->bind(commandBuffer);
        }
        if (currentMeshBuffer != entry.meshBuffer) {
            currentMeshBuffer = entry.meshBuffer;
            if (currentMeshBuffer) {
                currentMeshBuffer->bind(commandBuffer);
            }
        }
        if (currentMeshBuffer == nullptr || entry.meshBuffer->index_type() == duk::rhi::IndexType::NONE) {
            render_draw_indirect(commandBuffer, drawBuffer, entry, multiDrawSupported);
        } else {
            render_draw_indexed_indirect(commandBuffer, drawBuffer, entry, multiDrawSupported);
        }
    }
}

static void render_draw_data(duk::rhi::CommandBuffer* commandBuffer, const DrawData& drawData, bool multiDrawSupported) {
    // opaque
    render_draw_group(commandBuffer, drawData.drawBuffer.get(), drawData.opaqueGroup, multiDrawSupported);

    // transparent
    render_draw_group(commandBuffer, drawData.drawBuffer.get(), drawData.transparentGroup, multiDrawSupported);
}

}// namespace detail

void DrawGroupData::clear() {
    objectEntries.clear();
    sortedIndices.clear();
    instanceDrawEntries.clear();
    drawEntries.clear();
}

void DrawData::clear() {
    opaqueGroup.clear();
    transparentGroup.clear();
    drawBuffer->clear();
}

ForwardPass::ForwardPass(const ForwardPassCreateInfo& forwardPassCreateInfo)
    : m_rhi(forwardPassCreateInfo.rhi)
    , m_commandQueue(forwardPassCreateInfo.commandQueue)
    , m_outColor(duk::rhi::Access::COLOR_ATTACHMENT_WRITE, duk::rhi::PipelineStage::COLOR_ATTACHMENT_OUTPUT, duk::rhi::Image::Layout::SHADER_READ_ONLY) {
    {
        duk::rhi::AttachmentDescription colorAttachmentDescription = {};
        colorAttachmentDescription.format = detail::kColorFormat;
        colorAttachmentDescription.initialLayout = duk::rhi::Image::Layout::UNDEFINED;
        colorAttachmentDescription.layout = duk::rhi::Image::Layout::COLOR_ATTACHMENT;
        colorAttachmentDescription.finalLayout = duk::rhi::Image::Layout::SHADER_READ_ONLY;
        colorAttachmentDescription.storeOp = duk::rhi::StoreOp::STORE;
        colorAttachmentDescription.loadOp = duk::rhi::LoadOp::CLEAR;

        duk::rhi::AttachmentDescription depthAttachmentDescription = {};
        depthAttachmentDescription.format = m_rhi->capabilities()->depth_format();
        depthAttachmentDescription.initialLayout = duk::rhi::Image::Layout::UNDEFINED;
        depthAttachmentDescription.layout = duk::rhi::Image::Layout::DEPTH_STENCIL_ATTACHMENT;
        depthAttachmentDescription.finalLayout = duk::rhi::Image::Layout::DEPTH_STENCIL_ATTACHMENT;
        depthAttachmentDescription.storeOp = duk::rhi::StoreOp::DONT_CARE;
        depthAttachmentDescription.loadOp = duk::rhi::LoadOp::CLEAR;

        duk::rhi::AttachmentDescription attachmentDescriptions[] = {colorAttachmentDescription};

        duk::rhi::RHI::RenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.colorAttachments = attachmentDescriptions;
        renderPassCreateInfo.colorAttachmentCount = std::size(attachmentDescriptions);
        renderPassCreateInfo.depthAttachment = &depthAttachmentDescription;

        m_renderPass = m_rhi->create_render_pass(renderPassCreateInfo);
    }
    m_drawData.opaqueGroup.calculateSortKey = detail::calculate_opaque_sort_key;
    m_drawData.transparentGroup.calculateSortKey = detail::calculate_transparent_sort_key;

    {
        DrawBufferCreateInfo drawBufferCreateInfo = {};
        drawBufferCreateInfo.rhi = m_rhi;
        drawBufferCreateInfo.commandQueue = m_commandQueue;
        m_drawData.drawBuffer = std::make_unique<DrawBuffer>(drawBufferCreateInfo);
    }
}

ForwardPass::~ForwardPass() = default;

PassConnection* ForwardPass::out_color() {
    return &m_outColor;
}

void ForwardPass::update(const Pass::UpdateParams& params) {
    if (!m_colorImage || glm::vec2(m_colorImage->size()) != params.viewport) {
        m_colorImage = detail::create_color_image(m_rhi, m_commandQueue, params.viewport.x, params.viewport.y);

        // recreate frame buffer in case image was resized
        m_frameBuffer.reset();

        m_outColor.update(m_colorImage.get());
    }

    if (!m_depthImage || glm::vec2(m_depthImage->size()) != params.viewport) {
        m_depthImage = detail::create_depth_image(m_rhi, m_commandQueue, params.viewport.x, params.viewport.y);

        // recreate frame buffer in case image was resized
        m_frameBuffer.reset();
    }

    if (!m_frameBuffer) {
        duk::rhi::Image* frameBufferAttachments[] = {m_colorImage.get(), m_depthImage.get()};

        duk::rhi::RHI::FrameBufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.attachmentCount = std::size(frameBufferAttachments);
        frameBufferCreateInfo.attachments = frameBufferAttachments;
        frameBufferCreateInfo.renderPass = m_renderPass.get();

        m_frameBuffer = m_rhi->create_frame_buffer(frameBufferCreateInfo);
    }

    detail::update_draw_data(params, m_renderPass.get(), m_drawData);
}

void ForwardPass::render(duk::rhi::CommandBuffer* commandBuffer) {
    commandBuffer->begin_render_pass(m_renderPass.get(), m_frameBuffer.get());

    const auto isMultiDrawIndirectSupported = m_rhi->capabilities()->is_multi_draw_indirect_supported();

    detail::render_draw_data(commandBuffer, m_drawData, isMultiDrawIndirectSupported);

    commandBuffer->end_render_pass();
}

}// namespace duk::renderer