//
// Created by rov on 12/5/2023.
//

#include "duk_import/resource_set_importer.h"
#include "duk_log/log.h"
#include "duk_tools/file.h"

namespace duk::import {

namespace detail {

ResourceDescription parse_resource_description(const rapidjson::Value& object) {
    ResourceDescription resourceDescription = {};
    resourceDescription.tag = json::from_json_member<const char*>(object, "tag", "undefined");
    resourceDescription.path = json::from_json_member<const char*>(object, "file");
    resourceDescription.id = json::from_json_member<duk::resource::Id>(object, "id");
    auto aliasesMember = object.FindMember("aliases");
    if (aliasesMember != object.MemberEnd()) {
        auto aliases = aliasesMember->value.GetArray();
        for (auto& alias: aliases) {
            resourceDescription.aliases.insert(alias.GetString());
        }
    }
    return resourceDescription;
}

ResourceDescription load_resource_description(const std::filesystem::path& path) {
    auto content = duk::tools::File::load_text(path.string().c_str());

    rapidjson::Document document;

    auto& result = document.Parse(content.data());
    auto resourceDescription = parse_resource_description(result.GetObject());
    // convert relative path into absolute path
    resourceDescription.path = (path.parent_path() / resourceDescription.path).string();

    return resourceDescription;
}

}// namespace detail

ResourceSetImporter::~ResourceSetImporter() = default;

bool operator==(const ResourceDescription& lhs, const ResourceDescription& rhs) {
    return lhs.id == rhs.id;
}

bool operator<(const ResourceDescription& lhs, const ResourceDescription& rhs) {
    return lhs.id < rhs.id;
}

bool ResourceSetImporter::accept(const std::filesystem::path& directory) const {
    return std::filesystem::is_directory(directory);
}

ResourceSet ResourceSetImporter::load(const std::filesystem::path& directory) const {
    if (!std::filesystem::is_directory(directory)) {
        throw std::invalid_argument(fmt::format("path \"{}\" is not a directory", directory.string()));
    }

    ResourceSet resourceSet;

    // recursively search for .res files and add them to the resourceSet
    for (const auto& entry: std::filesystem::recursive_directory_iterator(directory)) {
        if (std::filesystem::is_regular_file(entry.path()) && entry.path().extension() == ".res") {
            auto resourceDescription = detail::load_resource_description(entry.path());

            // check if resource actually exists, ignore if not
            if (!std::filesystem::exists(resourceDescription.path)) {
                duk::log::warn("ignored resource at \"{0}\", file at \"{1}\" does not exist!", entry.path().string(), resourceDescription.path);
                continue;
            }

            // include resource description into result
            resourceSet.resources.emplace(resourceDescription.id, resourceDescription);
            for (auto& alias: resourceDescription.aliases) {
                resourceSet.aliases.emplace(alias, resourceDescription.id);
            }
        }
    }

    return resourceSet;
}

}// namespace duk::import
