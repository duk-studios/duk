/// 04/03/2024
/// resource.cpp

#include <duk_project/project.h>
#include <duk_project/resource.h>

#include <duk_import/importer.h>
#include <duk_import/resource_set_importer.h>
#include <duk_tools/file.h>

#include <fmt/format.h>

#include <fstream>
#include <random>

namespace duk::project {

namespace detail {

static bool is_scene_resource(const std::string& extension) {
    return extension == ".scn";
}

static bool is_image_resource(const std::string& extension) {
    return extension == ".png" || extension == ".jpg" || extension == ".jpeg";
}

static bool is_material_resource(const std::string& extension) {
    return extension == ".mat";
}

static bool is_resource(const std::string& extension) {
    return is_material_resource(extension) || is_image_resource(extension) || is_scene_resource(extension);
}

static std::string resource_tag(const std::string& extension) {
    if (is_material_resource(extension)) {
        return "mat";
    }
    if (is_image_resource(extension)) {
        return "img";
    }
    if (is_scene_resource(extension)) {
        return "scn";
    }
    throw std::invalid_argument(fmt::format("Unknown resource tag \"{}\"", extension));
}

static bool has_resource_id(const Project* project, const duk::resource::Id& id) {
    return project->resources.find(id) != project->resources.end();
}

static duk::resource::Id resource_id_generate(const Project* project) {
    static std::random_device rd;
    static std::mt19937 gen(rd());// mersenne_twister_engine seeded with rd()
    static std::uniform_int_distribution<uint64_t> distrib(duk::import::kMaxBuiltInResourceId.value() + 1, std::numeric_limits<uint64_t>::max());

    auto id = duk::resource::Id(distrib(gen));
    while (has_resource_id(project, id)) {
        // is it possible that we will loop forever?
        // naahhh, we're fine
        id = duk::resource::Id(distrib(gen));
    }

    return id;
}

static duk::import::ResourceDescription read_resource_description(const std::filesystem::path& path) {
    auto json = duk::tools::File::load_text(path.string().c_str());
    duk::serial::JsonReader reader(json.c_str());

    duk::import::ResourceDescription resourceDescription = {};
    reader.visit(resourceDescription);

    return resourceDescription;
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
    for (const auto& entry: std::filesystem::recursive_directory_iterator(project->root)) {
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
    for (auto resourceFile: resourceFiles) {
        auto trackFile = std::filesystem::path(resourceFile).replace_extension(".res");
        if (trackFiles.find(trackFile) != trackFiles.end()) {
            auto resourceDescription = detail::read_resource_description(trackFile);
            detail::add_resource(project, resourceDescription.id, resourceFile, trackFile);
        } else {
            untrackedResourceFiles.insert(resourceFile);
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

    duk::import::ResourceDescription resourceDescription = {};
    resourceDescription.file = std::filesystem::relative(resource, resource.parent_path()).string();
    resourceDescription.tag = detail::resource_tag(extension);
    resourceDescription.id = detail::resource_id_generate(project);

    duk::serial::JsonWriter writer;
    writer.visit(resourceDescription);

    auto trackFilePath = std::filesystem::path(resource).replace_extension(".res");
    std::ofstream file(trackFilePath);

    file << writer.pretty_print();

    detail::add_resource(project, resourceDescription.id, resource, trackFilePath);
}

}// namespace duk::project