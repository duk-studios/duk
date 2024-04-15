//
// Created by rov on 12/2/2023.
//

#include <duk_renderer/material/material_handler.h>

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

namespace duk::renderer {

namespace detail {

std::unique_ptr<MaterialDataSource> parse_material_json(const std::string& json) {
    duk::serial::JsonReader reader(json.c_str());

    duk::serial::detail::SerializedMaterial material;
    reader.visit(material);

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

    auto content = duk::tools::load_bytes(path);
    std::string jsonString(content.begin(), content.end());
    auto dataSource = detail::parse_material_json(jsonString.c_str());
    pool->create(id, dataSource.get());
}
}// namespace duk::renderer
