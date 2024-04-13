/// 13/11/2023
/// handler.cpp

#include <duk_resource/handler.h>

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

void ResourceHandler::solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) {
}

void ResourceHandler::solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) {
}

Handler::Handler(const HandlerCreateInfo& handlerCreateInfo)
    : m_pools(handlerCreateInfo.pools) {
}

Handler::~Handler() = default;

ResourceHandler* Handler::add_resource_handler(std::unique_ptr<ResourceHandler> resourceHandler) {
    const auto& tag = resourceHandler->tag();
    auto it = m_resourceHandlers.find(tag);
    if (it != m_resourceHandlers.end()) {
        throw std::logic_error(fmt::format("a ResourceHandler with tag \"{}\" already exists in this handler", tag));
    }
    return m_resourceHandlers.emplace(tag, std::move(resourceHandler)).first->second.get();
}

void Handler::load_resource_set(const std::filesystem::path& path) {
    std::filesystem::path directory = path;
    if (!std::filesystem::is_directory(path)) {
        directory = path.parent_path();
    }
    m_resourceSet = detail::load_resource_set(directory);
}

void Handler::load_resource(Id id) {
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

    auto handler = get_handler(resourceDescription.tag);
    if (!handler) {
        throw std::runtime_error(fmt::format("Could not find an handler for tag \"{}\"", resourceDescription.tag));
    }

    handler->load(resourceDescription.id, resourceDescription.file);

    DependencySolver dependencySolver;
    handler->solve_dependencies(resourceDescription.id, dependencySolver);

    for (auto dependency: dependencySolver.dependencies()) {
        load_resource(dependency);
    }

    ReferenceSolver referenceSolver(m_pools);
    handler->solve_references(resourceDescription.id, referenceSolver);
}

Id Handler::find_id(const std::string& alias) {
    auto& aliases = m_resourceSet.aliases;
    auto it = aliases.find(alias);
    if (it == aliases.end()) {
        throw std::invalid_argument(fmt::format("could not find resource id for alias \"{}\"", alias));
    }
    return it->second;
}

ResourceHandler* Handler::get_handler(const std::string& tag) {
    auto it = m_resourceHandlers.find(tag);
    if (it == m_resourceHandlers.end()) {
        return nullptr;
    }
    return it->second.get();
}

}// namespace duk::resource