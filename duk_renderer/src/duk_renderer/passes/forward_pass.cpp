/// 05/10/2023
/// forward_pass.cpp

#include "duk_renderer/components/transform.h"
#include <duk_renderer/brushes/mesh.h>
#include <duk_renderer/brushes/sprite_brush.h>
#include <duk_renderer/components/mesh_renderer.h>
#include <duk_renderer/components/sprite_renderer.h>
#include <duk_renderer/passes/forward_pass.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/resources/materials/material.h>
#include <duk_renderer/resources/materials/pipeline.h>

namespace duk::renderer {

namespace detail {

static constexpr auto kColorFormat = duk::rhi::PixelFormat::RGBA8U;

static SortKey calculate_mesh_sort_key(const MeshRenderer* param) {
    SortKey::Flags flags = {};
    flags.higher16Bits = reinterpret_cast<std::intptr_t>(param->material.get());
    flags.lower16Bits = reinterpret_cast<std::intptr_t>(param->mesh.get());
    return SortKey{flags};
}

static bool compatible(const MeshEntry& meshEntry, const MeshDrawEntry& drawEntry) {
    return meshEntry.mesh == drawEntry.mesh && meshEntry.material == drawEntry.material;
}

static bool valid(const MeshDrawEntry& drawEntry) {
    return drawEntry.material != nullptr && drawEntry.mesh != nullptr && drawEntry.instanceCount > 0;
}

static void render_meshes(const Pass::RenderParams& renderParams, duk::rhi::RenderPass* renderPass, MeshDrawData* drawData) {
    drawData->clear();
    auto& objects = renderParams.scene->objects();
    auto& meshes = drawData->meshes;
    auto& sortedMeshes = drawData->sortedMeshes;
    auto& drawEntries = drawData->drawEntries;

    std::set<Material*> uniqueMaterials;

    for (auto object: objects.all_with<MeshRenderer>()) {
        auto meshRenderer = object.component<MeshRenderer>();

        auto material = meshRenderer->material.get();

        if (!material->instance_buffer()) {
            duk::log::warn("MeshRenderer with a Material with no InstanceBuffer detected, skipping...");
            continue;
        }

        auto& objectEntry = meshes.emplace_back();
        objectEntry.objectId = object.id();
        objectEntry.mesh = meshRenderer->mesh.get();
        objectEntry.material = material;
        objectEntry.sortKey = calculate_mesh_sort_key(meshRenderer.get());

        uniqueMaterials.insert(material);
    }

    if (meshes.empty()) {
        return;
    }

    for (auto material: uniqueMaterials) {
        material->instance_buffer()->clear();
    }

    SortKey::sort_indices(meshes, sortedMeshes);

    MeshDrawEntry drawEntry = {};
    drawEntry.params.renderPass = renderPass;
    drawEntry.params.outputWidth = renderParams.outputWidth;
    drawEntry.params.outputHeight = renderParams.outputHeight;
    drawEntry.params.globalDescriptors = renderParams.globalDescriptors;

    for (auto sortedIndex: sortedMeshes) {
        auto& meshEntry = meshes[sortedIndex];

        // update instance buffer
        meshEntry.material->instance_buffer()->insert(objects.object(meshEntry.objectId));

        if (compatible(meshEntry, drawEntry)) {
            drawEntry.instanceCount++;
            continue;
        }

        if (valid(drawEntry)) {
            drawEntries.push_back(drawEntry);
        }

        // if the material is different, it's an entire new draw entry starting at 0
        if (drawEntry.material != meshEntry.material) {
            drawEntry.instanceCount = 1;
            drawEntry.firstInstance = 0;
        }
        // if only the mesh is different, just jump to the correct instance start
        else if (drawEntry.mesh != meshEntry.mesh) {
            drawEntry.firstInstance += drawEntry.instanceCount;
            drawEntry.instanceCount = 1;
        }
        drawEntry.material = meshEntry.material;
        drawEntry.mesh = meshEntry.mesh;
    }
    if (valid(drawEntry)) {
        drawEntries.push_back(drawEntry);
    }

    // mark all instance buffers for gpu upload
    for (auto material: uniqueMaterials) {
        material->instance_buffer()->flush();
    }

    // for each draw entry
    Material* currentMaterial = nullptr;
    for (auto& entry: drawEntries) {
        if (currentMaterial != entry.material) {
            currentMaterial = entry.material;
            entry.material->bind(renderParams.commandBuffer, entry.params);
        }
        entry.mesh->draw(renderParams.commandBuffer, entry.instanceCount, entry.firstInstance);
    }
}

static SortKey calculate_sprite_sort_key(Material* material) {
    SortKey sortKey = {};
    sortKey.flags.higher16Bits = reinterpret_cast<std::intptr_t>(material);
    sortKey.flags.lower16Bits = sortKey.flags.higher16Bits;
    return sortKey;
}

static bool compatible(const SpriteEntry& spriteEntry, const SpriteDrawEntry& drawEntry) {
    return spriteEntry.material == drawEntry.material;
}

static bool valid(const SpriteDrawEntry& drawEntry) {
    return drawEntry.material != nullptr && drawEntry.instanceCount > 0;
}

void render_sprites(const Pass::RenderParams& renderParams, duk::rhi::RenderPass* renderPass, SpriteDrawData* drawData) {
    drawData->clear();
    auto& objects = renderParams.scene->objects();
    auto& sortedSprites = drawData->sortedSprites;
    auto& sprites = drawData->sprites;
    auto& drawEntries = drawData->drawEntries;
    auto& brush = drawData->brush;

    std::set<Material*> usedMaterials;

    for (auto object: objects.all_with<SpriteRenderer>()) {
        auto spriteRenderer = object.component<SpriteRenderer>();
        auto material = spriteRenderer->material.get();
        auto sprite = spriteRenderer->sprite.get();

        usedMaterials.insert(material);

        SpriteEntry spriteEntry = {};
        spriteEntry.objectId = object.id();
        spriteEntry.sprite = sprite;
        spriteEntry.material = material;
        spriteEntry.sortKey = calculate_sprite_sort_key(material);
        sprites.push_back(spriteEntry);
    }

    if (sprites.empty()) {
        return;
    }

    SortKey::sort_indices(sprites, sortedSprites);

    SpriteDrawEntry drawEntry = {};
    drawEntry.brush = brush.get();

    for (auto sortedIndex: sortedSprites) {
        auto& spriteEntry = sprites[sortedIndex];

        auto object = objects.object(spriteEntry.objectId);

        brush->push(spriteEntry.sprite, model_matrix_3d(object));

        if (compatible(spriteEntry, drawEntry)) {
            drawEntry.instanceCount++;
            continue;
        }
        if (valid(drawEntry)) {
            drawEntries.push_back(drawEntry);
        }
        drawEntry.material = spriteEntry.material;
        drawEntry.firstInstance = drawEntry.firstInstance + drawEntry.instanceCount;
        drawEntry.instanceCount = 1;
    }
    // add last entry
    if (valid(drawEntry)) {
        drawEntries.push_back(drawEntry);
    }

    auto& commandBuffer = renderParams.commandBuffer;

    DrawParams drawParams = {};
    drawParams.globalDescriptors = renderParams.globalDescriptors;
    drawParams.outputWidth = renderParams.outputWidth;
    drawParams.outputHeight = renderParams.outputHeight;
    drawParams.renderPass = renderPass;

    Material* currentMaterial = nullptr;

    for (auto& entry: drawEntries) {
        if (entry.material != currentMaterial) {
            entry.material->bind(commandBuffer, drawParams);
            currentMaterial = entry.material;
        }
        entry.brush->draw(commandBuffer, entry.instanceCount, entry.firstInstance);
    }
}

}// namespace detail

void MeshDrawData::clear() {
    meshes.clear();
    sortedMeshes.clear();
    drawEntries.clear();
}

void SpriteDrawData::clear() {
    brush->clear();
    sprites.clear();
    sortedSprites.clear();
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

    {
        SpriteBrushCreateInfo spriteBrushCreateInfo = {};
        spriteBrushCreateInfo.initialSpriteCapacity = 10;
        spriteBrushCreateInfo.renderer = m_renderer;

        m_spriteDrawData.brush = std::make_unique<SpriteBrush>(spriteBrushCreateInfo);
    }
}

ForwardPass::~ForwardPass() = default;

void ForwardPass::render(const RenderParams& renderParams) {
    if (!m_colorImage || m_colorImage->width() != renderParams.outputWidth || m_colorImage->height() != renderParams.outputHeight) {
        m_colorImage = m_renderer->create_color_image(renderParams.outputWidth, renderParams.outputHeight, detail::kColorFormat);

        // recreate frame buffer in case image was resized
        m_frameBuffer.reset();

        m_outColor.update(m_colorImage.get());
    }

    if (!m_depthImage || m_depthImage->width() != renderParams.outputWidth || m_depthImage->height() != renderParams.outputHeight) {
        m_depthImage = m_renderer->create_depth_image(renderParams.outputWidth, renderParams.outputHeight);

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

    renderParams.commandBuffer->begin_render_pass(m_renderPass.get(), m_frameBuffer.get());

    detail::render_meshes(renderParams, m_renderPass.get(), &m_meshDrawData);

    detail::render_sprites(renderParams, m_renderPass.get(), &m_spriteDrawData);

    renderParams.commandBuffer->end_render_pass();
}

PassConnection* ForwardPass::out_color() {
    return &m_outColor;
}

}// namespace duk::renderer