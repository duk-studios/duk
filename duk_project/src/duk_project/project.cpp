/// 13/03/2024
/// project.cpp

#include <duk_project/project.h>

#include <duk_log/log.h>
#include <duk_tools/file.h>

#include <fstream>

namespace duk::project {

namespace detail {

static const auto kDukProjectFolder = ".duk";

static bool has_duk_project(const std::filesystem::path& path) {
    return std::filesystem::exists(path / kDukProjectFolder);
}

static std::filesystem::path find_project_root(std::filesystem::path path) {
    if (has_duk_project(path)) {
        return path;
    }
    std::filesystem::path root;
    while (path.has_parent_path() && path != path.root_path()) {
        if (has_duk_project(path)) {
            root = path;
            break;
        }
        path = path.parent_path();
    }
    return root;
}

static void write_project_settings(const Project* project) {
    duk::engine::Settings settings = project->settings;

    duk::serial::JsonWriter writer;
    writer.visit(settings);

    duk::tools::save_text(project->root / ".duk/settings.json", writer.pretty_print());
}

static void read_project_settings(Project* project) {
    auto settingsPath = project->root / ".duk/settings.json";
    if (!std::filesystem::exists(settingsPath)) {
        throw std::runtime_error(fmt::format("settings.json not found at {}", settingsPath.string()));
    }

    auto settingsJson = duk::tools::load_text(settingsPath);
    duk::serial::JsonReader reader(settingsJson.c_str());
    reader.visit(project->settings);
}

}// namespace detail

void init(Project* project, std::filesystem::path path) {
    if (!std::filesystem::is_directory(path)) {
        path = path.parent_path();
    }

    if (!std::filesystem::exists(path)) {
        throw std::invalid_argument(fmt::format("cannot initialize a duk project in an invalid resourceFile: \"{}\"", path.string()));
    }

    auto root = detail::find_project_root(path);
    if (!root.empty()) {
        throw std::invalid_argument(fmt::format("cannot initialize a duk project inside of another project, found root at \"{}\"", root.string()));
    }

    // create directory
    auto dukPath = path / detail::kDukProjectFolder;
    std::error_code errorCode;
    if (!std::filesystem::create_directory(dukPath, errorCode)) {
        throw std::runtime_error(fmt::format("failed to create .duk directory at \"{}\", reason: {}", dukPath.string(), errorCode.message()));
    }
    project->root = path;

    project->settings.name = "unnamed";
    project->settings.resolution = {1280, 720};
    project->settings.scene = duk::resource::kInvalidId;

    detail::write_project_settings(project);
}

void open(Project* project, std::filesystem::path path) {
    if (!std::filesystem::exists(path)) {
        throw std::invalid_argument(fmt::format("cannot open a duk project in an invalid resourceFile: \"{}\"", path.string()));
    }

    auto root = detail::find_project_root(path);
    if (root.empty()) {
        throw std::invalid_argument(fmt::format("\"{}\" does not contain a duk project, nor is inside of one", path.string()));
    }

    project->root = root;

    detail::read_project_settings(project);
}

void update(Project* project) {
    if (project->root.empty()) {
        throw std::invalid_argument("project root is not initialized");
    }

    auto untrackedFiles = resource_scan(project);
    if (untrackedFiles.empty()) {
        duk::log::info("Project is up-to-date, no untracked files found");
        return;
    }

    for (auto& untrackedFile: untrackedFiles) {
        duk::log::info("Adding track file for {}", untrackedFile.string());
        resource_track(project, untrackedFile);
    }
}

}// namespace duk::project