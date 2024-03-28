/// 13/11/2023
/// importer.cpp

#include <duk_resource/importer.h>

#include <duk_log/log.h>

#include <duk_tools/file.h>

namespace duk::resource {

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

static ResourceSet load_resource_set(const std::filesystem::path& directory) {
    if (!std::filesystem::is_directory(directory)) {
        throw std::invalid_argument(fmt::format("path \"{}\" is not a directory", directory.string()));
    }

    ResourceSet resourceSet;

    // recursively search for .res files and add them to the resourceSet
    for (const auto& entry: std::filesystem::recursive_directory_iterator(directory)) {
        if (std::filesystem::is_regular_file(entry.path()) && entry.path().extension() == ".res") {
            auto resourceDescription = load_resource_description(entry.path());

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

}// namespace detail

bool operator==(const ResourceDescription& lhs, const ResourceDescription& rhs) {
    return lhs.id == rhs.id;
}

bool operator<(const ResourceDescription& lhs, const ResourceDescription& rhs) {
    return lhs.id < rhs.id;
}

void ResourceImporter::solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) {
}

void ResourceImporter::solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) {
}

Importer::Importer(const ImporterCreateInfo& importerCreateInfo)
    : m_pools(importerCreateInfo.pools) {
}

Importer::~Importer() = default;

ResourceImporter* Importer::add_resource_importer(std::unique_ptr<ResourceImporter> resourceImporter) {
    const auto& tag = resourceImporter->tag();
    auto it = m_resourceImporters.find(tag);
    if (it != m_resourceImporters.end()) {
        throw std::logic_error(fmt::format("a ResourceImporter with tag \"{}\" already exists in this importer", tag));
    }
    return m_resourceImporters.emplace(tag, std::move(resourceImporter)).first->second.get();
}

void Importer::load_resource_set(const std::filesystem::path& path) {
    std::filesystem::path directory = path;
    if (!std::filesystem::is_directory(path)) {
        directory = path.parent_path();
    }
    m_resourceSet = detail::load_resource_set(directory);
}

void Importer::load_resource(Id id) {
    if (id < kMaxBuiltInResourceId) {
        // this is a builtin resource, skip
        return;
    }

    auto& resources = m_resourceSet.resources;

    auto resourceIt = resources.find(id);
    if (resourceIt == resources.end()) {
        throw std::invalid_argument("resource id not found");
    }

    const auto& resourceDescription = resourceIt->second;

    auto importer = get_importer(resourceDescription.tag);
    if (!importer) {
        throw std::runtime_error(fmt::format("Could not find an importer for tag \"{}\"", resourceDescription.tag));
    }

    importer->load(resourceDescription.id, resourceDescription.file);

    DependencySolver dependencySolver;
    importer->solve_dependencies(resourceDescription.id, dependencySolver);

    for (auto dependency: dependencySolver.dependencies()) {
        load_resource(dependency);
    }

    ReferenceSolver referenceSolver(m_pools);
    importer->solve_references(resourceDescription.id, referenceSolver);
}

Id Importer::find_id(const std::string& alias) {
    auto& aliases = m_resourceSet.aliases;
    auto it = aliases.find(alias);
    if (it == aliases.end()) {
        throw std::invalid_argument(fmt::format("could not find resource id for alias \"{}\"", alias));
    }
    return it->second;
}

ResourceImporter* Importer::get_importer(const std::string& tag) {
    auto it = m_resourceImporters.find(tag);
    if (it == m_resourceImporters.end()) {
        return nullptr;
    }
    return it->second.get();
}

}// namespace duk::resource