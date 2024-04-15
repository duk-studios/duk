/// 04/03/2024
/// resource.cpp

#include <duk_project/project.h>
#include <duk_project/resource.h>

#include <duk_resource/handler.h>
#include <duk_tools/file.h>

#include <fmt/format.h>

#include <fstream>
#include <random>

namespace duk::project {

namespace detail {

static bool is_resource(const std::string& extension) {
    const auto handler = duk::resource::ResourceRegistry::instance()->find_handler_for_extension(extension);
    return handler != nullptr;
}

static std::string resource_tag(const std::string& extension) {
    if (const auto handler = duk::resource::ResourceRegistry::instance()->find_handler_for_extension(extension)) {
        return handler->tag();
    }
    throw std::invalid_argument(fmt::format("Unknown resource tag \"{}\"", extension));
}

static bool has_resource_id(const Project* project, const duk::resource::Id& id) {
    return project->resources.contains(id);
}

static duk::resource::Id resource_id_generate(const Project* project) {
    static std::random_device rd;
    static std::mt19937 gen(rd());// mersenne_twister_engine seeded with rd()
    static std::uniform_int_distribution<uint64_t> distrib(duk::resource::kMaxBuiltInResourceId.value() + 1, std::numeric_limits<uint64_t>::max());

    auto id = duk::resource::Id(distrib(gen));
    while (has_resource_id(project, id)) {
        // is it possible that we will loop forever?
        // naahhh, we're fine
        id = duk::resource::Id(distrib(gen));
    }

    return id;
}

static duk::resource::ResourceFile read_resource_file(const std::filesystem::path& path) {
    auto json = duk::tools::load_text(path);
    std::string jsonString(json.begin(), json.end());
    duk::serial::JsonReader reader(jsonString.c_str());

    duk::resource::ResourceFile resourceFile = {};
    reader.visit(resourceFile);

    return resourceFile;
}

static void add_resource(Project* project, const duk::resource::Id& id, const std::filesystem::path& resourceFile, const std::filesystem::path& trackFile) {
    if (has_resource_id(project, id)) {
        throw std::invalid_argument(fmt::format("tried to add duplicate resource id ({})", id.value()));
    }
    project->resources.emplace(id, ResourceEntry{resourceFile, trackFile});
}

}// namespace detail

std::set<std::filesystem::path> resource_scan(Project* project) {
    if (!std::filesystem::exists(project->root)) {
        throw std::invalid_argument(fmt::format("resource scan failed: \"{}\" does not exist", project->root.string()));
    }

    std::set<std::filesystem::path> trackFiles;
    std::set<std::filesystem::path> resourceFiles;
    for (const auto& entry: std::filesystem::recursive_directory_iterator(project->root / "resources")) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const auto file = entry.path();
        const auto extension = file.extension();
        if (extension == ".res") {
            trackFiles.insert(file);
        } else if (detail::is_resource(extension.string())) {
            resourceFiles.insert(entry);
        }
    }

    std::set<std::filesystem::path> untrackedResourceFiles;
    for (auto resourceFilepath: resourceFiles) {
        auto trackFile = std::filesystem::path(resourceFilepath).replace_extension(".res");
        if (trackFiles.find(trackFile) != trackFiles.end()) {
            auto resourceFile = detail::read_resource_file(trackFile);
            detail::add_resource(project, resourceFile.id, resourceFilepath, trackFile);
        } else {
            untrackedResourceFiles.insert(resourceFilepath);
        }
    }

    return untrackedResourceFiles;
}

void resource_track(Project* project, const std::filesystem::path& resource) {
    if (!std::filesystem::exists(resource)) {
        throw std::invalid_argument(fmt::format("resource \"{}\" does not exist", resource.string()));
    }

    const auto extension = resource.extension().string();

    if (!detail::is_resource(extension)) {
        throw std::invalid_argument(fmt::format("resource \"{}\" is not a resource", resource.string()));
    }

    duk::resource::ResourceFile resourceFile = {};
    resourceFile.file = std::filesystem::relative(resource, resource.parent_path()).string();
    resourceFile.tag = detail::resource_tag(extension);
    resourceFile.id = detail::resource_id_generate(project);

    duk::serial::JsonWriter writer;
    writer.visit(resourceFile);

    auto trackFilePath = std::filesystem::path(resource).replace_extension(".res");
    std::ofstream file(trackFilePath);

    file << writer.pretty_print();

    detail::add_resource(project, resourceFile.id, resource, trackFilePath);
}

}// namespace duk::project