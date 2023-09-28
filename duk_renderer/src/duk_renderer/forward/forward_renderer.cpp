/// 01/08/2023
/// forward_renderer.cpp

#include <duk_renderer/forward/forward_renderer.h>
#include <duk_renderer/components/mesh_painter.h>

#include <duk_rhi/pipeline/std_shader_data_source.h>

#include <fstream>
#include <numeric>

namespace duk::renderer {


struct SortKey {
    struct Flags {
        uint32_t painterValue;
        uint16_t meshValue;
        uint16_t paletteValue;
    };
    union {
        Flags flags;
        uint64_t key;
    };

};

namespace detail {

SortKey sort_key_for_mesh_painter(const duk::scene::Component<MeshPainter>& meshPainter) {
    SortKey::Flags flags = {};
    flags.meshValue = reinterpret_cast<std::intptr_t>(meshPainter->mesh);
    flags.painterValue = reinterpret_cast<std::intptr_t>(meshPainter->painter);
    flags.paletteValue = reinterpret_cast<std::intptr_t>(meshPainter->palette);
    return SortKey{flags};
}

}

ForwardRenderer::ForwardRenderer(const ForwardRendererCreateInfo& forwardRendererCreateInfo) :
    Renderer(forwardRendererCreateInfo.rendererCreateInfo) {

    {
        m_depthImage = create_depth_image(render_width(), render_height());
    }

    {
        duk::rhi::AttachmentDescription colorAttachmentDescription = {};
        colorAttachmentDescription.format = m_rhi->present_image()->format();
        colorAttachmentDescription.initialLayout = duk::rhi::Image::Layout::UNDEFINED;
        colorAttachmentDescription.layout = duk::rhi::Image::Layout::COLOR_ATTACHMENT;
        colorAttachmentDescription.finalLayout = duk::rhi::Image::Layout::PRESENT_SRC;
        colorAttachmentDescription.storeOp = duk::rhi::StoreOp::STORE;
        colorAttachmentDescription.loadOp = duk::rhi::LoadOp::CLEAR;

        duk::rhi::AttachmentDescription depthAttachmentDescription = {};
        depthAttachmentDescription.format = m_depthImage->format();
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

        auto expectedRenderPass = m_rhi->create_render_pass(renderPassCreateInfo);

        if (!expectedRenderPass) {
            throw std::runtime_error("failed to create RenderPass: " + expectedRenderPass.error().description());
        }

        CanvasCreateInfo canvasCreateInfo = {};
        canvasCreateInfo.renderPass = std::move(expectedRenderPass.value());
        canvasCreateInfo.width = render_width();
        canvasCreateInfo.height = render_height();

        m_canvas = std::make_unique<Canvas>(canvasCreateInfo);
    }

    {
        duk::rhi::Image* frameBufferAttachments[] = {m_rhi->present_image(), m_depthImage.get()};

        duk::rhi::RHI::FrameBufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.attachmentCount = std::size(frameBufferAttachments);
        frameBufferCreateInfo.attachments = frameBufferAttachments;
        frameBufferCreateInfo.renderPass = m_canvas->render_pass();

        auto expectedFrameBuffer = m_rhi->create_frame_buffer(frameBufferCreateInfo);

        if (!expectedFrameBuffer) {
            throw std::runtime_error("failed to create FrameBuffer: " + expectedFrameBuffer.error().description());
        }

        m_frameBuffer = std::move(expectedFrameBuffer.value());
    }
}

void ForwardRenderer::render(duk::scene::Scene* scene) {

    m_rhi->prepare_frame();

    m_scheduler->begin();

    auto mainRenderPass = m_mainQueue->submit([this, scene](duk::rhi::CommandBuffer* commandBuffer) {
        commandBuffer->begin();

        duk::rhi::CommandBuffer::RenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.frameBuffer = m_frameBuffer.get();
        renderPassBeginInfo.renderPass = m_canvas->render_pass();

        commandBuffer->begin_render_pass(renderPassBeginInfo);

        struct ObjectEntry {
            duk::scene::Object::Id objectId;
            Mesh* mesh;
            Painter* painter;
            Palette* palette;
            SortKey sortKey;
        };

        std::vector<ObjectEntry> objectEntries;
        for (auto object : scene->objects_with_components<MeshPainter>()) {
            auto meshPainter = object.component<MeshPainter>();

            ObjectEntry objectEntry = {};
            objectEntry.objectId = object.id();
            objectEntry.mesh = meshPainter->mesh;
            objectEntry.painter = meshPainter->painter;
            objectEntry.palette = meshPainter->palette;
            objectEntry.sortKey = detail::sort_key_for_mesh_painter(meshPainter);

            objectEntries.push_back(objectEntry);
        }

        // sort objects
        std::vector<uint16_t> sortedObjectIndices(objectEntries.size());

        // fill vector with 0 ... objectsEntries.size()
        std::iota(sortedObjectIndices.begin(), sortedObjectIndices.end(), 0);

        // sort indices based on SortKey
        std::sort(sortedObjectIndices.begin(), sortedObjectIndices.end(), [&objectEntries](uint16_t lhs, uint16_t rhs) {
            return objectEntries[lhs].sortKey.key < objectEntries[rhs].sortKey.key;
        });

        struct PaintEntry {
            Painter::PaintParams params;
            Painter* painter;
        };

        auto compatible_with_paint_entry = [](const PaintEntry& paintEntry, const ObjectEntry& objectEntry) -> bool {
            return paintEntry.painter == objectEntry.painter && paintEntry.params.mesh == objectEntry.mesh && paintEntry.params.palette == objectEntry.palette;
        };

        auto is_valid = [](const PaintEntry& paintEntry) {
            return paintEntry.painter && paintEntry.params.mesh && paintEntry.params.palette && paintEntry.params.instanceCount > 0;
        };

        std::vector<PaintEntry> paintEntries;

        PaintEntry paintEntry = {};
        paintEntry.params.canvas = m_canvas.get();

        for (auto sortedIndex : sortedObjectIndices) {
            auto& objectEntry = objectEntries[sortedIndex];

            // if this object belongs to another paint entry
            if (!compatible_with_paint_entry(paintEntry, objectEntry)) {

                // if this entry has an actual painter
                if (is_valid(paintEntry)) {
                    paintEntries.push_back(paintEntry);
                }
                if (paintEntry.params.palette != objectEntry.palette || paintEntry.painter != objectEntry.painter) {
                    paintEntry.params.instanceCount = 0;
                    paintEntry.params.firstInstance = 0;
                }
                else if (paintEntry.params.mesh != objectEntry.mesh) {
                    paintEntry.params.firstInstance += paintEntry.params.instanceCount;
                    paintEntry.params.instanceCount = 0;
                }
                paintEntry.painter = objectEntry.painter;
                paintEntry.params.palette = objectEntry.palette;
                paintEntry.params.mesh = objectEntry.mesh;
            }

            Palette::InsertInstanceParams instanceParams = {};
            instanceParams.object = scene->object(objectEntry.objectId);

            objectEntry.palette->insert_instance(instanceParams);

            paintEntry.params.instanceCount++;
        }
        if (is_valid(paintEntry)) {
            paintEntries.push_back(paintEntry);
        }

        // for each painter entry
        for (auto& [params, painter] : paintEntries) {
            painter->paint(commandBuffer, params);
        }

        commandBuffer->end_render_pass();

        commandBuffer->end();
    });

    auto acquireImageCommand = m_scheduler->schedule(m_rhi->acquire_image_command());
    auto mainRenderPassCommand = m_scheduler->schedule(std::move(mainRenderPass));
    auto presentCommand = m_scheduler->schedule(m_rhi->present_command());

    mainRenderPassCommand.wait(acquireImageCommand, duk::rhi::PipelineStage::COLOR_ATTACHMENT_OUTPUT);

    presentCommand.wait(mainRenderPassCommand);

    m_scheduler->flush();
}

void ForwardRenderer::resize(uint32_t width, uint32_t height) {
    Renderer::resize(width, height);
    duk::rhi::EmptyImageDataSource depthImageDataSource(render_width(), render_height(), m_rhi->capabilities()->depth_format());
    depthImageDataSource.update_hash();
    m_depthImage->update(&depthImageDataSource);

    m_canvas->resize(width, height);
}

}
