//
// Created by rov on 12/5/2023.
//

#include <duk_import/resource_set_importer.h>
#include <duk_log/log.h>
#include <duk_tools/file.h>

namespace duk::import {

namespace detail {

ResourceDescription load_resource_description(const std::filesystem::path& path) {
    auto content = duk::tools::File::load_text(path.string().c_str());

    duk::serial::JsonReader reader(content.c_str());

    ResourceDescription resourceDescription = {};
    reader.visit(resourceDescription);
    // convert relative path into absolute path
    resourceDescription.file = (path.parent_path() / resourceDescription.file).string();

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
            if (!std::filesystem::exists(resourceDescription.file)) {
                duk::log::warn("ignored resource at \"{0}\", file at \"{1}\" does not exist!", entry.path().string(), resourceDescription.file);
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
