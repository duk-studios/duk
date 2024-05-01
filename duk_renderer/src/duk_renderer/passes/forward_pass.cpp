/// 05/10/2023
/// forward_pass.cpp

#include <duk_renderer/components/mesh_renderer.h>
#include <duk_renderer/components/sprite_renderer.h>
#include <duk_renderer/components/text_renderer.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/material/material.h>
#include <duk_renderer/material/pipeline.h>
#include <duk_renderer/mesh/mesh.h>
#include <duk_renderer/passes/forward_pass.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/sprite/sprite_cache.h>
#include <duk_renderer/sprite/sprite_mesh.h>

namespace duk::renderer {

namespace detail {

static constexpr auto kColorFormat = duk::rhi::PixelFormat::RGBA8U;

static void update_texts(const Pass::UpdateParams& params, duk::rhi::RenderPass* renderPass, TextDrawData* drawData) {
    drawData->clear();
    auto objects = params.objects;
    auto& drawEntries = drawData->drawEntries;

    std::set<Material*> uniqueMaterials;

    for (auto object: objects->all_with<TextRenderer>()) {
        auto textRenderer = object.component<TextRenderer>();

        if (!textRenderer->material || !textRenderer->mesh) {
            duk::log::warn("TextRenderer with no material and/or mesh, missing call to update_text_renderer");
            continue;
        }

        auto material = textRenderer->material.get();
        uniqueMaterials.insert(material);

        TextDrawEntry textEntry = {};
        textEntry.material = material;
        textEntry.mesh = textRenderer->mesh.get();

        drawEntries.emplace_back(textEntry);
    }

    // update materials
    DrawParams drawParams = {};
    drawParams.globalDescriptors = params.globalDescriptors;
    drawParams.outputWidth = params.outputWidth;
    drawParams.outputHeight = params.outputHeight;
    drawParams.renderPass = renderPass;

    for (auto material: uniqueMaterials) {
        material->update(drawParams);
    }
}

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

static void update_meshes(const Pass::UpdateParams& params, duk::rhi::RenderPass* renderPass, MeshDrawData* drawData) {
    drawData->clear();
    auto objects = params.objects;
    auto& meshes = drawData->meshes;
    auto& sortedMeshes = drawData->sortedMeshes;
    auto& drawEntries = drawData->drawEntries;

    std::set<Material*> uniqueMaterials;

    for (auto object: objects->all_with<MeshRenderer, Transform>()) {
        auto meshRenderer = object.component<MeshRenderer>();

        auto material = meshRenderer->material.get();

        if (!meshRenderer->material) {
            duk::log::warn("MeshRenderer with no Material, skipping...");
            continue;
        }

        if (!meshRenderer->mesh) {
            duk::log::warn("MeshRenderer with no Mesh, skipping...");
            continue;
        }

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

    for (auto sortedIndex: sortedMeshes) {
        auto& meshEntry = meshes[sortedIndex];

        // update instance buffer
        meshEntry.material->instance_buffer()->insert(objects->object(meshEntry.objectId));

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

    // update materials
    DrawParams drawParams = {};
    drawParams.globalDescriptors = params.globalDescriptors;
    drawParams.outputWidth = params.outputWidth;
    drawParams.outputHeight = params.outputHeight;
    drawParams.renderPass = renderPass;

    for (auto material: uniqueMaterials) {
        // flush instance buffers
        material->instance_buffer()->flush();
        material->update(drawParams);
    }
}

static SortKey calculate_sprite_sort_key(const Material* material, const SpriteMesh* mesh) {
    SortKey sortKey = {};
    sortKey.flags.higher16Bits = reinterpret_cast<std::intptr_t>(material);
    sortKey.flags.lower16Bits = reinterpret_cast<std::intptr_t>(mesh);
    return sortKey;
}

static bool compatible(const SpriteEntry& spriteEntry, const SpriteDrawEntry& drawEntry) {
    return spriteEntry.material == drawEntry.material && spriteEntry.mesh == drawEntry.mesh;
}

static bool valid(const SpriteDrawEntry& drawEntry) {
    return drawEntry.material && drawEntry.mesh && drawEntry.instanceCount;
}

static void update_sprites(const Pass::UpdateParams& params, duk::rhi::RenderPass* renderPass, SpriteDrawData* drawData) {
    drawData->clear();
    auto objects = params.objects;
    auto& sortedSprites = drawData->sortedSprites;
    auto& sprites = drawData->sprites;
    auto& drawEntries = drawData->drawEntries;
    auto& cache = drawData->cache;

    std::set<Material*> uniqueMaterials;

    for (auto object: objects->all_with<SpriteRenderer>()) {
        auto spriteRenderer = object.component<SpriteRenderer>();

        spriteRenderer->material = cache->material_for(spriteRenderer->sprite.get());
        spriteRenderer->mesh = cache->mesh_for(spriteRenderer->sprite.get(), spriteRenderer->index);

        const auto material = spriteRenderer->material.get();
        const auto mesh = spriteRenderer->mesh.get();

        uniqueMaterials.insert(spriteRenderer->material.get());

        SpriteEntry spriteEntry = {};
        spriteEntry.objectId = object.id();
        spriteEntry.mesh = mesh;
        spriteEntry.material = material;
        spriteEntry.sortKey = calculate_sprite_sort_key(material, mesh);
        sprites.push_back(spriteEntry);
    }

    if (sprites.empty()) {
        return;
    }

    for (auto material: uniqueMaterials) {
        material->instance_buffer()->clear();
    }

    SortKey::sort_indices(sprites, sortedSprites);

    SpriteDrawEntry drawEntry = {};

    for (auto sortedIndex: sortedSprites) {
        auto& spriteEntry = sprites[sortedIndex];

        // update instance buffer
        spriteEntry.material->instance_buffer()->insert(objects->object(spriteEntry.objectId));

        if (compatible(spriteEntry, drawEntry)) {
            drawEntry.instanceCount++;
            continue;
        }
        if (valid(drawEntry)) {
            drawEntries.push_back(drawEntry);
        }
        // if the material is different, it's an entire new draw entry starting at 0
        if (drawEntry.material != spriteEntry.material) {
            drawEntry.instanceCount = 1;
            drawEntry.firstInstance = 0;
        }
        // if only the mesh is different, just jump to the correct instance start
        else if (drawEntry.mesh != spriteEntry.mesh) {
            drawEntry.firstInstance += drawEntry.instanceCount;
            drawEntry.instanceCount = 1;
        }
        drawEntry.material = spriteEntry.material;
        drawEntry.mesh = spriteEntry.mesh;
    }
    // add last entry
    if (valid(drawEntry)) {
        drawEntries.push_back(drawEntry);
    }

    DrawParams drawParams = {};
    drawParams.globalDescriptors = params.globalDescriptors;
    drawParams.outputWidth = params.outputWidth;
    drawParams.outputHeight = params.outputHeight;
    drawParams.renderPass = renderPass;

    for (auto& material: uniqueMaterials) {
        material->instance_buffer()->flush();
        material->update(drawParams);
    }
}

}// namespace detail

void MeshDrawData::clear() {
    meshes.clear();
    sortedMeshes.clear();
    drawEntries.clear();
}

void SpriteDrawData::clear() {
    sprites.clear();
    sortedSprites.clear();
    drawEntries.clear();
}

void TextDrawData::clear() {
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
        SpriteCacheCreateInfo spriteCacheCreateInfo = {};
        spriteCacheCreateInfo.renderer = m_renderer;

        m_spriteDrawData.cache = std::make_unique<SpriteCache>(spriteCacheCreateInfo);
    }
}

ForwardPass::~ForwardPass() = default;

PassConnection* ForwardPass::out_color() {
    return &m_outColor;
}

void ForwardPass::update(const Pass::UpdateParams& params) {
    if (!m_colorImage || m_colorImage->width() != params.outputWidth || m_colorImage->height() != params.outputHeight) {
        m_colorImage = m_renderer->create_color_image(params.outputWidth, params.outputHeight, detail::kColorFormat);

        // recreate frame buffer in case image was resized
        m_frameBuffer.reset();

        m_outColor.update(m_colorImage.get());
    }

    if (!m_depthImage || m_depthImage->width() != params.outputWidth || m_depthImage->height() != params.outputHeight) {
        m_depthImage = m_renderer->create_depth_image(params.outputWidth, params.outputHeight);

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

    detail::update_texts(params, m_renderPass.get(), &m_textDrawData);

    detail::update_meshes(params, m_renderPass.get(), &m_meshDrawData);

    detail::update_sprites(params, m_renderPass.get(), &m_spriteDrawData);
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

    // render sprites
    {
        Material* currentMaterial = nullptr;
        for (auto& entry: m_spriteDrawData.drawEntries) {
            if (entry.material != currentMaterial) {
                entry.material->bind(commandBuffer);
                currentMaterial = entry.material;
            }
            entry.mesh->draw(commandBuffer, entry.instanceCount, entry.firstInstance);
        }
    }

    // render texts
    {
        for (auto& entry: m_textDrawData.drawEntries) {
            entry.material->bind(commandBuffer);
            entry.mesh->draw(commandBuffer);
        }
    }

    commandBuffer->end_render_pass();
}

}// namespace duk::renderer