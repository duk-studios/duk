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

namespace duk::renderer {

namespace detail {

static constexpr auto kColorFormat = duk::rhi::PixelFormat::RGBA8U;

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

static void update_draw_group(DrawGroupData& drawGroup) {
    SortKey::sort_indices(drawGroup.objectEntries, drawGroup.sortedIndices);

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
            drawGroup.drawEntries.push_back(drawEntry);
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
        drawGroup.drawEntries.push_back(drawEntry);
    }
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

    update_draw_group(drawData.opaqueGroup);
    update_draw_group(drawData.transparentGroup);

    for (const auto material: uniqueMaterials) {
        material->update(*params.pipelineCache, renderPass, params.viewport);
    }
}

static void render_draw_group(duk::rhi::CommandBuffer* commandBuffer, const DrawGroupData& drawGroup) {
    Material* currentMaterial = nullptr;
    for (auto& entry: drawGroup.drawEntries) {
        if (currentMaterial != entry.material) {
            currentMaterial = entry.material;
            entry.material->bind(commandBuffer);
        }
        entry.mesh->draw(commandBuffer, entry.instanceCount, entry.firstInstance);
    }
}

}// namespace detail

void DrawGroupData::clear() {
    objectEntries.clear();
    sortedIndices.clear();
    drawEntries.clear();
}

void DrawData::clear() {
    opaqueGroup.clear();
    transparentGroup.clear();
}

ForwardPass::ForwardPass(const ForwardPassCreateInfo& forwardPassCreateInfo)
    : m_renderer(forwardPassCreateInfo.renderer)
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
        depthAttachmentDescription.format = m_renderer->rhi()->capabilities()->depth_format();
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

        m_renderPass = m_renderer->rhi()->create_render_pass(renderPassCreateInfo);
    }
    m_drawData.opaqueGroup.calculateSortKey = detail::calculate_opaque_sort_key;
    m_drawData.transparentGroup.calculateSortKey = detail::calculate_transparent_sort_key;
}

ForwardPass::~ForwardPass() = default;

PassConnection* ForwardPass::out_color() {
    return &m_outColor;
}

void ForwardPass::update(const Pass::UpdateParams& params) {
    if (!m_colorImage || glm::vec2(m_colorImage->size()) != params.viewport) {
        m_colorImage = m_renderer->create_color_image(params.viewport.x, params.viewport.y, detail::kColorFormat);

        // recreate frame buffer in case image was resized
        m_frameBuffer.reset();

        m_outColor.update(m_colorImage.get());
    }

    if (!m_depthImage || glm::vec2(m_depthImage->size()) != params.viewport) {
        m_depthImage = m_renderer->create_depth_image(params.viewport.x, params.viewport.y);

        // recreate frame buffer in case image was resized
        m_frameBuffer.reset();
    }

    if (!m_frameBuffer) {
        duk::rhi::Image* frameBufferAttachments[] = {m_colorImage.get(), m_depthImage.get()};

        duk::rhi::RHI::FrameBufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.attachmentCount = std::size(frameBufferAttachments);
        frameBufferCreateInfo.attachments = frameBufferAttachments;
        frameBufferCreateInfo.renderPass = m_renderPass.get();

        m_frameBuffer = m_renderer->rhi()->create_frame_buffer(frameBufferCreateInfo);
    }

    detail::update_draw_data(params, m_renderPass.get(), m_drawData);
}

void ForwardPass::render(duk::rhi::CommandBuffer* commandBuffer) {
    commandBuffer->begin_render_pass(m_renderPass.get(), m_frameBuffer.get());

    // render opaque
    detail::render_draw_group(commandBuffer, m_drawData.opaqueGroup);

    // render transparent
    detail::render_draw_group(commandBuffer, m_drawData.transparentGroup);

    commandBuffer->end_render_pass();
}

}// namespace duk::renderer