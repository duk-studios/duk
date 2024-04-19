//
// Created by rov on 12/2/2023.
//

#include <duk_renderer/material/material_handler.h>

#include <duk_tools/file.h>

namespace duk::renderer {

struct SerializedMaterial {
    std::string type;
    std::unique_ptr<duk::renderer::MaterialDataSource> dataSource;
};

}// namespace duk::renderer

namespace duk::serial {

namespace detail {

template<typename T>
std::unique_ptr<T> read_material_data_source(const rapidjson::Value& json) {
    auto dataSource = std::make_unique<T>();
    from_json(json, *dataSource);
    dataSource->update_hash();
    return dataSource;
}

}// namespace detail

template<>
inline void from_json<duk::renderer::SerializedMaterial>(const rapidjson::Value& json, duk::renderer::SerializedMaterial& material) {
    from_json_member(json, "type", material.type);
    if (material.type == "Color") {
        material.dataSource = detail::read_material_data_source<duk::renderer::ColorMaterialDataSource>(json);
        return;
    }
    if (material.type == "Phong") {
        material.dataSource = detail::read_material_data_source<duk::renderer::PhongMaterialDataSource>(json);
        return;
    }
    throw std::runtime_error(fmt::format("unknown material type: \"{}\"", material.type));
}

}// namespace duk::serial

namespace duk::renderer {

namespace detail {

std::unique_ptr<MaterialDataSource> parse_material_json(const std::string& json) {
    SerializedMaterial material;
    duk::serial::read_json(json, material);
    return std::move(material.dataSource);
}

}// namespace detail

MaterialHandler::MaterialHandler()
    : ResourceHandlerT("mat") {
}

bool MaterialHandler::accepts(const std::string& extension) const {
    return extension == ".mat";
}

void MaterialHandler::load(MaterialPool* pool, const resource::Id& id, const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error(fmt::format("material at ({}) does not exist", path.string()));
    }

    if (path.extension() != ".mat") {
        throw std::runtime_error(fmt::format("tried to load material at ({}), which does not have the correct extension", path.string()));
    }

    auto jsonContent = duk::tools::File::load_text(path.string().c_str());
    auto dataSource = detail::parse_material_json(jsonContent);
    pool->create(id, dataSource.get());
}
}// namespace duk::renderer
