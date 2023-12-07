//
// Created by rov on 12/6/2023.
//

#include <duk_import/resource/resource_importer_json.h>
#include <duk_import/json/types.h>
#include <duk_tools/file.h>

#include <rapidjson/document.h>

namespace duk::import {

namespace detail {

ResourceType parse_resource_type(const char* typeName) {
    if (strcmp(typeName, "res") == 0) {
        return ResourceType::RES;
    }
    if (strcmp(typeName, "img") == 0) {
        return ResourceType::IMG;
    }
    if (strcmp(typeName, "mat") == 0) {
        return ResourceType::MAT;
    }
    throw std::runtime_error("invalid resource type");
}

ResourceDescription parse_resource_description(const std::filesystem::path& directory, const rapidjson::Value& object) {
    ResourceDescription resourceDescription = {};
    resourceDescription.type = parse_resource_type(object["type"].GetString());
    resourceDescription.path = (directory / object["path"].GetString()).string();
    resourceDescription.id = json::to_resource_id(object["id"]);
    return resourceDescription;
}

ResourceSet parse_resource_set(const std::filesystem::path& directory, const rapidjson::Value& object) {
    auto resources = object["resources"].GetArray();

    ResourceSet resourceSet;

    for (auto& resource : resources) {
        auto resourceDescription = detail::parse_resource_description(directory, resource.GetObject());

        switch (resourceDescription.type) {
            case ResourceType::RES: resourceSet.resourceSets.insert(resourceDescription); break;
            case ResourceType::IMG: resourceSet.images.insert(resourceDescription); break;
            case ResourceType::MAT: resourceSet.materials.insert(resourceDescription); break;
        }
    }

    return resourceSet;
}

ResourceSet load(const std::filesystem::path& filepath) {
    auto content = duk::tools::File::load_text(filepath.string().c_str());

    rapidjson::Document document;

    auto& result = document.Parse(content.data());

    auto root = result.GetObject();

    auto directory = filepath.parent_path();
    auto rootResourceSet = detail::parse_resource_set(directory, root);

    for (const auto& childResourceSetDescription : rootResourceSet.resourceSets) {
        auto childPath = directory / childResourceSetDescription.path;

        auto child = load(childPath);

        rootResourceSet.images.insert(child.images.begin(), child.images.end());
        rootResourceSet.materials.insert(child.materials.begin(), child.materials.end());
    }

    return rootResourceSet;
}

}

bool ResourceImporterJson::accept(const std::filesystem::path& path) const {
    auto filename = path.filename();
    return filename == "resources.json";
}

ResourceSet ResourceImporterJson::load(const std::filesystem::path& path) const {
    return detail::load(path);
}

}