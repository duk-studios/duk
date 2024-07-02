/// 04/03/2024
/// resource.cpp

#include <duk_project/project.h>
#include <duk_project/resource.h>

#include <duk_resource/handler.h>
#include <duk_tools/file.h>

#include <duk_serial/json/serializer.h>

#include <fstream>

namespace duk::project {

namespace detail {

static bool is_resource(const Project* project, const std::string& extension) {
    const auto handler = duk::resource::ResourceRegistry::instance()->find_handler_for_extension(extension);
    if (handler) {
        return true;
    }

    const auto& settings = project->settings;
    for (auto& resourceExtension: settings.resourceExtensions) {
        if (resourceExtension == extension) {
            return true;
        }
    }
    return false;
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

static void add_resource(Project* project, const duk::resource::Id& id, const std::filesystem::path& dataFile, const std::filesystem::path& resourceFile) {
    if (has_resource_id(project, id)) {
        throw std::invalid_argument(fmt::format("tried to add duplicate resource id ({})", id.value()));
    }
    project->resources.emplace(id, ResourceEntry{dataFile, resourceFile});
}

}// namespace detail

duk::resource::ResourceFile load_resource_file(const std::filesystem::path& path) {
    const auto json = duk::tools::load_text(path);
    return duk::serial::read_json<duk::resource::ResourceFile>(json);
}

std::set<std::filesystem::path> resource_scan(Project* project) {
    if (!std::filesystem::exists(project->root)) {
        throw std::invalid_argument(fmt::format("resource scan failed: \"{}\" does not exist", project->root.string()));
    }

    std::set<std::filesystem::path> resourceFiles;
    std::set<std::filesystem::path> dataFiles;
    for (const auto& entry: std::filesystem::recursive_directory_iterator(project->root / "resources")) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const auto file = entry.path();
        const auto extension = file.extension().string();
        if (extension == ".res") {
            resourceFiles.insert(file);
        } else if (detail::is_resource(project, extension)) {
            dataFiles.insert(file);
        }
    }

    std::set<std::filesystem::path> untrackedDataFiles;
    for (const auto& dataFile: dataFiles) {
        auto resourceFilePath = std::filesystem::path(dataFile).replace_extension(".res");
        if (resourceFiles.contains(resourceFilePath)) {
            auto resourceFile = load_resource_file(resourceFilePath);
            detail::add_resource(project, resourceFile.id, dataFile, resourceFilePath);
        } else {
            untrackedDataFiles.insert(dataFile);
        }
    }

    return untrackedDataFiles;
}

duk::resource::Id resource_track(Project* project, const std::filesystem::path& resource) {
    if (!std::filesystem::exists(resource)) {
        throw std::invalid_argument(fmt::format("resource \"{}\" does not exist", resource.string()));
    }

    const auto extension = resource.extension().string();

    if (!detail::is_resource(project, extension)) {
        throw std::invalid_argument(fmt::format("resource \"{}\" is not a resource", resource.string()));
    }

    duk::resource::ResourceFile resourceFile = {};
    resourceFile.file = std::filesystem::relative(resource, resource.parent_path()).string();
    resourceFile.tag = detail::resource_tag(extension);
    resourceFile.id = detail::resource_id_generate(project);

    std::ostringstream oss;
    duk::serial::write_json(oss, resourceFile, true);

    auto trackFilePath = std::filesystem::path(resource).replace_extension(".res");
    std::ofstream file(trackFilePath);

    file << oss.str();

    detail::add_resource(project, resourceFile.id, resource, trackFilePath);

    return resourceFile.id;
}

}// namespace duk::project