//
// Created by rov on 12/4/2023.
//

#include <duk_import/material/material_importer_json.h>
#include <duk_json/types.h>
#include <duk_tools/file.h>
#include <duk_renderer/pools/material_pool.h>

#include <fstream>
#include <regex>

namespace duk::import {

namespace detail {

template<typename T>
std::unique_ptr<T> create_material_from_json(const rapidjson::Value& object) {
    auto material = json::make_unique_from_json<T>(object);
    material->update_hash();
    return material;
}

std::unique_ptr<duk::renderer::MaterialDataSource> parse_material(const rapidjson::Value& object) {
    const auto type = json::from_json_member<const char*>(object, "type", "");
    if (strcmp(type, "Color") == 0) {
        return create_material_from_json<duk::renderer::ColorMaterialDataSource>(object);
    }
    if (strcmp(type, "Phong") == 0) {
        return create_material_from_json<duk::renderer::PhongMaterialDataSource>(object);
    }
    throw std::runtime_error("unsupported material for parsing");
}

}

bool MaterialImporterJson::accepts(const std::filesystem::path& path) {
    return path.extension() == ".mat";
}

std::unique_ptr<duk::renderer::MaterialDataSource> MaterialImporterJson::load(const std::filesystem::path& path) {

    auto content = duk::tools::File::load_text(path.string().c_str());

    rapidjson::Document document;

    auto& result = document.Parse(content.data());

    auto root = result.GetObject();

    return detail::parse_material(root);
}

}
