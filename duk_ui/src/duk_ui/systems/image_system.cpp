//
// Created by Ricardo on 26/08/2024.
//

#include <duk_renderer/renderer.h>
#include <duk_renderer/components/material_slot.h>
#include <duk_renderer/components/mesh_slot.h>
#include <duk_renderer/mesh/dynamic_mesh.h>
#include <duk_ui/components/canvas.h>
#include <duk_ui/components/image.h>
#include <duk_ui/systems/image_system.h>

namespace duk::ui {

namespace detail {

uint64_t calculate_image_material_hash(const Image& image) {
    uint64_t hash = 0;
    duk::hash::hash_combine(hash, image.image.id());
    duk::hash::hash_combine(hash, image.sampler);
    return hash;
}

uint64_t calculate_image_hash(const CanvasTransform& canvasTransform, const Image& image) {
    uint64_t hash = 0;
    duk::hash::hash_combine(hash, canvasTransform.hash);
    duk::hash::hash_combine(hash, image.image.id());
    duk::hash::hash_combine(hash, image.sampler);
    duk::hash::hash_combine(hash, image.color);
    return hash;
}

duk::resource::Handle<duk::renderer::DynamicMesh> create_mesh(const duk::renderer::Renderer* renderer) {
    duk::renderer::DynamicMeshCreateInfo dynamicMeshCreateInfo = {};
    dynamicMeshCreateInfo.attributes = duk::renderer::VertexAttributes({
        duk::renderer::VertexAttributes::POSITION,
        duk::renderer::VertexAttributes::UV
    });
    dynamicMeshCreateInfo.indexType = duk::rhi::IndexType::UINT16;
    dynamicMeshCreateInfo.maxVertexCount = 4;
    dynamicMeshCreateInfo.maxIndexCount = 6;
    dynamicMeshCreateInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::DYNAMIC;
    dynamicMeshCreateInfo.meshBufferPool = renderer->mesh_buffer_pool();
    return std::make_shared<duk::renderer::DynamicMesh>(dynamicMeshCreateInfo);
}

}

void ImageSystem::update() {
    for (auto [canvasTransform, image] : all_components_of<CanvasTransform, Image>()) {
        // check for hash changes
        const auto hash = detail::calculate_image_hash(*canvasTransform, *image);
        if (hash == image->hash) {
            continue;
        }
        image->hash = hash;

        const auto renderer = global<duk::renderer::Renderer>();

        update_material(image);

        // build mesh
        {
            const auto min = canvasTransform->localMin;
            const auto max = canvasTransform->localMax;

            auto meshSlot = image.component_or_add<duk::renderer::MeshSlot>();
            auto mesh = meshSlot->mesh.as<duk::renderer::DynamicMesh>();
            if (!mesh) {
                mesh = detail::create_mesh(renderer);
                meshSlot->mesh = mesh;
            }
            mesh->clear();

            glm::vec3 positions[] = {
                {min.x, min.y, 0.0f},
                {max.x, min.y, 0.0f},
                {min.x, max.y, 0.0f},
                {max.x, max.y, 0.0f}
            };

            glm::vec2 uvs[] = {
                {0.0, 0.0},
                {1.0, 0.0},
                {0.0, 1.0},
                {1.0, 1.0}
            };

            uint16_t indices[] = {
                0, 1, 2,
                1, 3, 2
            };

            mesh->write_vertices(duk::renderer::VertexAttributes::POSITION, positions);
            mesh->write_vertices(duk::renderer::VertexAttributes::UV, uvs);
            mesh->write_indices(indices);
            mesh->flush();
        }
    }
}

void ImageSystem::update_material(const duk::objects::Component<Image>& image) {
    const auto materialSlot = image.component_or_add<duk::renderer::MaterialSlot>();
    const auto hash = detail::calculate_image_material_hash(*image);
    auto it = m_materials.find(hash);
    if (it == m_materials.end()) {
        auto material = duk::renderer::create_image_material(global<duk::renderer::Renderer>(), global<duk::renderer::Builtins>());
        material->set("uImage", image->image->descriptor(image->sampler));
        it = m_materials.emplace(hash, material).first;
    }

    materialSlot->material = it->second;
    materialSlot->material->set(materialSlot.id(), "uProperties", "color", image->color);
}

}
