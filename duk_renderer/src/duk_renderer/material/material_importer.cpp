//
// Created by rov on 12/2/2023.
//

#include <duk_renderer/material/material_importer.h>

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

MaterialImporter::MaterialImporter(const MaterialImporterCreateInfo& materialImporterCreateInfo)
    : m_materialPool(materialImporterCreateInfo.materialPool) {
}

const std::string& MaterialImporter::tag() const {
    static const std::string matTag("mat");
    return matTag;
}

void MaterialImporter::load(const duk::resource::Id& id, const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error(fmt::format("material at ({}) does not exist", path.string()));
    }

    if (path.extension() != ".mat") {
        throw std::runtime_error(fmt::format("tried to load material at ({}), which does not have the correct extension", path.string()));
    }

    auto jsonContent = duk::tools::File::load_text(path.string().c_str());
    auto dataSource = detail::parse_material_json(jsonContent);
    m_materialPool->create(id, dataSource.get());
}

void MaterialImporter::solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) {
    auto material = m_materialPool->find(id);
    if (!material) {
        throw std::logic_error("tried to solve dependencies of resource that was not loaded");
    }
    dependencySolver.solve(*material);
}

void MaterialImporter::solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) {
    auto material = m_materialPool->find(id);
    if (!material) {
        throw std::logic_error("tried to solve references of resource that was not loaded");
    }
    referenceSolver.solve(*material);
}

}// namespace duk::renderer
