//
// Created by rov on 12/4/2023.
//

#include <duk_import/material/material_loader_json.h>
#include <duk_renderer/pools/material_pool.h>
#include <duk_tools/file.h>

namespace duk::serial {

namespace detail {

struct SerializedMaterial {
    std::string type;
    std::unique_ptr<duk::renderer::MaterialDataSource> dataSource;
};

template<typename T>
std::unique_ptr<T> read_material_data_source(JsonReader* reader) {
    auto dataSource = std::make_unique<T>();
    visit_object(reader, *dataSource);
    dataSource->update_hash();
    return dataSource;
}

}// namespace detail

template<>
inline void visit_object(JsonReader* reader, detail::SerializedMaterial& material) {
    reader->visit_member(material.type, MemberDescription("type"));
    if (material.type == "Color") {
        material.dataSource = detail::read_material_data_source<duk::renderer::ColorMaterialDataSource>(reader);
        return;
    }
    if (material.type == "Phong") {
        material.dataSource = detail::read_material_data_source<duk::renderer::PhongMaterialDataSource>(reader);
        return;
    }
    throw std::runtime_error(fmt::format("unknown material type: \"{}\"", material.type));
}

}// namespace duk::serial

namespace duk::import {

namespace detail {

std::unique_ptr<duk::renderer::MaterialDataSource> parse_material_json(const std::string& json) {
    duk::serial::JsonReader reader(json.c_str());

    duk::serial::detail::SerializedMaterial material;
    reader.visit(material);

    return std::move(material.dataSource);
}

}// namespace detail

bool MaterialLoaderJson::accepts(const std::filesystem::path& path) {
    return path.extension() == ".mat";
}

std::unique_ptr<duk::renderer::MaterialDataSource> MaterialLoaderJson::load(const std::filesystem::path& path) {
    auto content = duk::tools::File::load_text(path.string().c_str());

    return detail::parse_material_json(content);
}

}// namespace duk::import
