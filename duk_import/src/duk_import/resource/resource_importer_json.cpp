//
// Created by rov on 12/6/2023.
//

#include <duk_import/resource/resource_importer_json.h>
#include <duk_json/types.h>
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
    if (strcmp(typeName, "scn") == 0) {
        return ResourceType::SCN;
    }
    throw std::invalid_argument("invalid resource type");
}

ResourceDescription parse_resource_description(const std::filesystem::path& path, const rapidjson::Value& object) {
    ResourceDescription resourceDescription = {};
    resourceDescription.type = parse_resource_type(object["type"].GetString());
    resourceDescription.path = (path.parent_path() / object["file"].GetString()).string();
    resourceDescription.id = json::from_json<duk::pool::ResourceId>(object["id"]);
    auto dependenciesMember = object.FindMember("dependencies");
    if (dependenciesMember != object.MemberEnd()) {
        auto dependencies = dependenciesMember->value.GetArray();
        for (auto& dependency : dependencies) {
            resourceDescription.dependencies.insert(json::from_json<duk::pool::ResourceId>(dependency));
        }
    }
    auto aliasesMember = object.FindMember("aliases");
    if (aliasesMember != object.MemberEnd()) {
        auto aliases = aliasesMember->value.GetArray();
        for (auto& alias : aliases) {
            resourceDescription.aliases.insert(alias.GetString());
        }
    }
    return resourceDescription;
}

ResourceDescription load_resource_description(const std::filesystem::path& path) {
    auto content = duk::tools::File::load_text(path.string().c_str());

    rapidjson::Document document;

    auto& result = document.Parse(content.data());

    return parse_resource_description(path, result.GetObject());
}

ResourceSet parse_resource_set(const std::filesystem::path& directory, const rapidjson::Value& object) {
    auto resources = object["resources"].GetArray();

    ResourceSet resourceSet;

    for (auto& resource : resources) {
        auto resourcePath = directory / resource.GetString();
        auto resourceDescription = load_resource_description(resourcePath);
        for (auto& alias : resourceDescription.aliases) {
            resourceSet.aliases.emplace(alias, resourceDescription.id);
        }
        resourceSet.resources.emplace(resourceDescription.id, std::move(resourceDescription));
    }

    return resourceSet;
}

ResourceSet load_resources(const std::filesystem::path& filepath) {
    auto content = duk::tools::File::load_text(filepath.string().c_str());

    rapidjson::Document document;

    auto& result = document.Parse(content.data());

    auto root = result.GetObject();

    auto directory = filepath.parent_path();
    auto rootResourceSet = detail::parse_resource_set(directory, root);

    return rootResourceSet;
}

}

bool ResourceImporterJson::accept(const std::filesystem::path& path) const {
    return path.filename() == "resources.json";
}

ResourceSet ResourceImporterJson::load(const std::filesystem::path& path) const {
    return detail::load_resources(path);
}

}