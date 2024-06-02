/// 05/10/2023
/// forward_pass.cpp

#include <duk_renderer/components/material_slot.h>
#include <duk_renderer/components/mesh_slot.h>
#include <duk_renderer/material/material.h>
#include <duk_renderer/mesh/mesh.h>
#include <duk_renderer/passes/forward_pass.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/material/globals/global_descriptors.h>

namespace duk::renderer {

namespace detail {

static constexpr auto kColorFormat = duk::rhi::PixelFormat::RGBA8U;

static SortKey calculate_sort_key(const MeshSlot* meshSlot, const MaterialSlot* materialSlot) {
    SortKey::Flags flags = {};
    flags.higher16Bits = reinterpret_cast<std::intptr_t>(materialSlot->material.get());
    flags.lower16Bits = reinterpret_cast<std::intptr_t>(meshSlot->mesh.get());
    return SortKey{flags};
}

static bool compatible(const MeshEntry& meshEntry, const MeshDrawEntry& drawEntry) {
    return meshEntry.mesh == drawEntry.mesh && meshEntry.material == drawEntry.material;
}

static bool valid(const MeshDrawEntry& drawEntry) {
    return drawEntry.material != nullptr && drawEntry.mesh != nullptr && drawEntry.instanceCount > 0;
}

static void update_meshes(const Pass::UpdateParams& params, duk::rhi::RenderPass* renderPass, MeshDrawData* drawData) {
    drawData->clear();
    auto objects = params.objects;
    auto& meshes = drawData->meshEntries;
    auto& sortedMeshes = drawData->sortedMeshEntries;
    auto& drawEntries = drawData->drawEntries;

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

        auto& objectEntry = meshes.emplace_back();
        objectEntry.objectId = object.id();
        objectEntry.mesh = meshSlot->mesh.get();
        objectEntry.material = materialSlot->material.get();
        objectEntry.sortKey = calculate_sort_key(meshSlot.get(), materialSlot.get());

        uniqueMaterials.insert(materialSlot->material.get());
    }

    if (meshes.empty()) {
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

    SortKey::sort_indices(meshes, sortedMeshes);

    MeshDrawEntry drawEntry = {};

    for (auto sortedIndex: sortedMeshes) {
        auto& meshEntry = meshes[sortedIndex];

        auto material = meshEntry.material;

        // update instance buffers
        material->push(meshEntry.objectId);

        if (compatible(meshEntry, drawEntry)) {
            drawEntry.instanceCount++;
            continue;
        }

        if (valid(drawEntry)) {
            drawEntries.push_back(drawEntry);
        }

        // if the material is different, it's an entire new draw entry starting at 0
        if (drawEntry.material != material) {
            drawEntry.instanceCount = 1;
            drawEntry.firstInstance = 0;
        }
        // if only the mesh is different, just jump to the correct instance start
        else if (drawEntry.mesh != meshEntry.mesh) {
            drawEntry.firstInstance += drawEntry.instanceCount;
            drawEntry.instanceCount = 1;
        }
        drawEntry.material = material;
        drawEntry.mesh = meshEntry.mesh;
    }
    if (valid(drawEntry)) {
        drawEntries.push_back(drawEntry);
    }

    for (auto material: uniqueMaterials) {
        material->update(*params.pipelineCache, renderPass, params.viewport);
    }
}

}// namespace detail

void MeshDrawData::clear() {
    meshEntries.clear();
    sortedMeshEntries.clear();
    drawEntries.clear();
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

    detail::update_meshes(params, m_renderPass.get(), &m_meshDrawData);
}

void ForwardPass::render(duk::rhi::CommandBuffer* commandBuffer) {
    commandBuffer->begin_render_pass(m_renderPass.get(), m_frameBuffer.get());

    // render meshes
    {
        Material* currentMaterial = nullptr;
        for (auto& entry: m_meshDrawData.drawEntries) {
            if (currentMaterial != entry.material) {
                currentMaterial = entry.material;
                entry.material->bind(commandBuffer);
            }
            entry.mesh->draw(commandBuffer, entry.instanceCount, entry.firstInstance);
        }
    }

    commandBuffer->end_render_pass();
}

}// namespace duk::renderer