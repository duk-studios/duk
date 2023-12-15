//
// Created by rov on 12/4/2023.
//

#include <duk_import/material/material_importer_json.h>
#include <duk_import/json/types.h>
#include <duk_tools/file.h>

#include <fstream>
#include <regex>

namespace duk::import {

namespace detail {

duk::renderer::MaterialType parse_material_type(const char* typeStr) {
    if (strcmp(typeStr, "Color") == 0) {
        return renderer::MaterialType::COLOR;
    }
    if (strcmp(typeStr, "Phong") == 0) {
        return renderer::MaterialType::PHONG;
    }
    if (strcmp(typeStr, "Fullscreen") == 0) {
        return renderer::MaterialType::FULLSCREEN;
    }
    return renderer::MaterialType::UNDEFINED;
}

std::unique_ptr<duk::renderer::ColorMaterialDataSource> parse_color_material(const rapidjson::Value& object) {
    auto material = std::make_unique<duk::renderer::ColorMaterialDataSource>();
    material->color = json::to_vec4(object["base-color"]);
    material->update_hash();
    return material;
}

std::unique_ptr<duk::renderer::PhongMaterialDataSource> parse_phong_material(const rapidjson::Value& object, const duk::renderer::ImagePool* imagePool) {
    auto material = std::make_unique<duk::renderer::PhongMaterialDataSource>();
    material->baseColor = json::to_vec4(object["base-color"]);
    material->baseColorImage = imagePool->find(json::to_resource_id(object["base-color-image"]));
    material->baseColorSampler = json::to_sampler(object["base-color-sampler"]);
    material->shininess = object["shininess"].GetFloat();
    material->shininessImage = imagePool->find(json::to_resource_id(object["shininess-image"]));
    material->shininessSampler = json::to_sampler(object["shininess-sampler"]);
    material->update_hash();
    return material;
}

std::unique_ptr<duk::renderer::MaterialDataSource> parse_material(const rapidjson::Value& object, const duk::renderer::ImagePool* imagePool) {
    const auto type = detail::parse_material_type(object["type"].GetString());
    switch (type) {
        case duk::renderer::MaterialType::COLOR:
            return parse_color_material(object);
        case duk::renderer::MaterialType::PHONG:
            return parse_phong_material(object, imagePool);
        default:
            throw std::runtime_error("unsupported material for parsing");
    }
}

}

MaterialImporterJson::MaterialImporterJson(const duk::renderer::ImagePool* imagePool) :
    m_imagePool(imagePool) {

}

bool MaterialImporterJson::accepts(const std::filesystem::path& path) {
    return path.extension() == ".mat";
}

std::unique_ptr<duk::renderer::MaterialDataSource> MaterialImporterJson::load(const std::filesystem::path& path) {

    auto content = duk::tools::File::load_text(path.string().c_str());

    rapidjson::Document document;

    auto& result = document.Parse(content.data());

    auto root = result.GetObject();

    return detail::parse_material(root, m_imagePool);
}

}
