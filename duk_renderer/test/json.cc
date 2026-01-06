//
// Created by rov on 02/01/2026.
//


#include <duk_renderer/material/material_data.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/components/lighting.h>

#include <catch2/catch_test_macros.hpp>

#include "duk_renderer/components/camera.h"
#include "duk_renderer/components/material_slot.h"
#include "duk_renderer/components/mesh_slot.h"
#include "duk_renderer/components/sprite_renderer.h"

TEST_CASE("Renderer objects can be serialized as json", "[renderer][json]") {
    using namespace duk::renderer;
    SECTION("Material serialization") {
        MaterialData input = {};
        input.shader = duk::resource::Id(42);
        {
            const auto bufferBinding = std::make_shared<BufferBinding>();
            BufferBinding::Member member1 = {};
            member1.type = BufferBinding::Member::Type::FLOAT;
            member1.name = "roughness";
            member1.data.floatValue = 0.5f;
            bufferBinding->members.push_back(member1);
            Binding binding1 = {};
            binding1.name = "materialProperties";
            binding1.type = BindingType::UNIFORM;
            binding1.data = bufferBinding;
            input.bindings.push_back(binding1);
        }
        {
            const auto imageSamplerBinding = std::make_shared<ImageSamplerBinding>();
            imageSamplerBinding->image = duk::resource::Handle<Image>(duk::resource::Id(84));
            imageSamplerBinding->sampler = duk::rhi::Sampler(duk::rhi::Sampler::Filter::LINEAR, duk::rhi::Sampler::WrapMode::REPEAT);
            Binding binding2 = {};
            binding2.name = "albedoTexture";
            binding2.type = BindingType::IMAGE_SAMPLER;
            binding2.data = imageSamplerBinding;
            input.bindings.push_back(binding2);
        }
        {
            const auto imageBinding = std::make_shared<ImageBinding>();
            imageBinding->image = duk::resource::Handle<Image>(duk::resource::Id(168));
            Binding binding3 = {};
            binding3.name = "normalMap";
            binding3.type = BindingType::IMAGE;
            binding3.data = imageBinding;
            input.bindings.push_back(binding3);
        }
        const auto json = duk::serial::json_write(input);
        INFO("MaterialData json: " << json);
        const auto output = duk::serial::json_read<MaterialData>(json);
        CHECK(input.shader.id() == output.shader.id());
        REQUIRE(input.bindings.size() == output.bindings.size());
        for (size_t i = 0; i < input.bindings.size(); i++) {
            const auto& inBinding = input.bindings[i];
            const auto& outBinding = output.bindings[i];
            CHECK(inBinding.name == outBinding.name);
            CHECK(inBinding.type == outBinding.type);
        }
    }
    SECTION("Texture serialization") {
        const auto input = Texture(ImageResource(duk::resource::Id(4)), duk::rhi::Sampler(duk::rhi::Sampler::Filter::CUBIC, duk::rhi::Sampler::WrapMode::MIRRORED_REPEAT));
        const auto json = duk::serial::json_write(input);
        INFO("Texture handle json: " << json);
        const auto output = duk::serial::json_read<Texture>(json);
        CHECK(input.image().id() == output.image().id());
        CHECK(input.sampler() == output.sampler());
    }
    SECTION("Component serialization") {
        SECTION("Transform") {
            const auto input = Transform{
                .position = {1.0f, 2.0f, 3.0f},
                .rotation = {0.0f, 0.0f, 0.0f, 1.0f},
                .scale = {1.0f, 1.0f, 1.0f}
            };
            const auto json = duk::serial::json_write(input);
            INFO("Transform json: " << json);
            const auto output = duk::serial::json_read<Transform>(json);
            CHECK(input.position == output.position);
            // Due to floating point precision, we check that the difference is small
            CHECK(glm::length(input.rotation - output.rotation) < 0.0001f);
            CHECK(input.scale == output.scale);
        }
        SECTION("PointLight") {
            const auto input = PointLight {
                .value = {
                    .color = glm::vec3(0.8f, 0.7f, 0.6f),
                    .intensity = 5.0f
                },
                .radius = 10.0f,
            };
            const auto json = duk::serial::json_write(input);
            INFO("Point light json: " << json);
            const auto output = duk::serial::json_read<PointLight>(json);
            CHECK(input.value.color == output.value.color);
            CHECK(input.value.intensity == output.value.intensity);
            CHECK(input.radius == output.radius);
        }
        SECTION("Directional light") {
            const auto input = DirectionalLight {
                .value = {
                    .color = glm::vec3(1.0f, 1.0f, 0.9f),
                    .intensity = 2.0f
                }
            };
            const auto json = duk::serial::json_write(input);
            INFO("Directional light json: " << json);
            const auto output = duk::serial::json_read<DirectionalLight>(json);
            CHECK(input.value.color == output.value.color);
            CHECK(input.value.intensity == output.value.intensity);
        }
        SECTION("PerspectiveCamera") {
            const auto input = PerspectiveCamera {
                .fovDegrees = glm::radians(60.0f),
                .zNear = 0.1f,
                .zFar = 1000.0f
            };
            const auto json = duk::serial::json_write(input);
            INFO("PerspectiveCamera json: " << json);
            const auto output = duk::serial::json_read<PerspectiveCamera>(json);
            CHECK(input.fovDegrees == output.fovDegrees);
            CHECK(input.zNear == output.zNear);
            CHECK(input.zFar == output.zFar);
        }
        SECTION("MaterialSlot") {
            const auto input = MaterialSlot {
                .material = duk::resource::Handle<Material>(duk::resource::Id(1234))
            };
            const auto json = duk::serial::json_write(input);
            INFO("MaterialSlot json: " << json);
            const auto output = duk::serial::json_read<MaterialSlot>(json);
            CHECK(input.material.id() == output.material.id());
        }
        SECTION("MeshSlot") {
            const auto input = MeshSlot {
                .mesh = duk::resource::Handle<Mesh>(duk::resource::Id(5678))
            };
            const auto json = duk::serial::json_write(input);
            INFO("MeshSlot json: " << json);
            const auto output = duk::serial::json_read<MeshSlot>(json);
            CHECK(input.mesh.id() == output.mesh.id());
        }
        SECTION("SpriteRenderer") {
            const auto input = SpriteRenderer {
                .sprite = duk::resource::Handle<Sprite>(duk::resource::Id(91011)),
                .index = 3
            };
            const auto json = duk::serial::json_write(input);
            INFO("SpriteRenderer json: " << json);
            const auto output = duk::serial::json_read<SpriteRenderer>(json);
            CHECK(input.sprite.id() == output.sprite.id());
            CHECK(input.index == output.index);
        }
    }
}