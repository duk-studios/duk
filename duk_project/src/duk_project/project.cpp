/// 13/03/2024
/// project.cpp

#include <duk_project/project.h>

#include <duk_log/log.h>
#include <duk_tools/file.h>

#include <duk_serial/json/serializer.h>

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

    std::ostringstream oss;
    duk::serial::write_json(oss, settings, true);

    duk::tools::save_text(project->root / "settings.json", oss.str());
}

static void read_project_settings(Project* project) {
    auto settingsPath = project->root / "settings.json";
    if (!std::filesystem::exists(settingsPath)) {
        throw std::runtime_error(fmt::format("settings.json not found at {}", settingsPath.string()));
    }

    auto settingsJson = duk::tools::load_text(settingsPath);
    duk::serial::read_json(settingsJson, project->settings);
}

static void pack_binaries(const std::filesystem::path& packPath, const std::filesystem::path& buildPath) {
    auto binariesPath = buildPath / "install/bin";

    std::filesystem::copy(binariesPath, packPath, std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive);
}

static void pack_resources(Project* project, const std::filesystem::path& packPath) {
    resource_scan(project);

    auto resourcesPath = packPath / "resources";
    if (!std::filesystem::exists(resourcesPath)) {
        std::filesystem::create_directories(resourcesPath);
    }

    std::vector<duk::resource::ResourceFile> resourceFiles;
    resourceFiles.reserve(project->resources.size());
    for (const auto& [id, entry]: project->resources) {
        auto resourceFile = load_resource_file(entry.resourceFile);
        resourceFile.file = fmt::format("{}.bin", id.value());
        resourceFiles.emplace_back(resourceFile);
        auto resourceData = duk::tools::load_bytes(entry.dataFile);

        duk::tools::save_compressed_bytes(resourcesPath / resourceFile.file, resourceData.data(), resourceData.size());
    }

    std::ostringstream oss;
    duk::serial::write_json(oss, resourceFiles);

    auto json = oss.str();

    auto resourcesBinPath = resourcesPath / "resources.bin";

    duk::tools::save_compressed_text(resourcesBinPath, json);
}

static void pack_settings(const Project* project, const std::filesystem::path& packPath) {
    duk::engine::Settings settings = project->settings;

    std::ostringstream oss;
    duk::serial::write_json(oss, settings, false);

    const auto settingsPath = packPath / "settings.bin";
    duk::tools::save_compressed_text(settingsPath, oss.str());
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
        auto id = resource_track(project, untrackedFile);
        duk::log::info("Adding resource file for '{}', id: '{}'", untrackedFile.string(), id.value());
    }
}

void build(Project* project, const std::string_view& generator, const std::filesystem::path& buildPath, const std::string_view& config, const std::string_view& additionalCmakeOptions) {
    if (project->root.empty()) {
        throw std::invalid_argument("project root is not initialized");
    }

    if (!std::filesystem::exists(buildPath)) {
        std::filesystem::create_directories(buildPath);
    }

    {
        std::ostringstream oss;
        oss << "cmake -G \"" << generator << "\" -S \"" << project->root.string() << "\" -B \"" << buildPath.string() << "\" "
            << " -DCMAKE_INSTALL_PREFIX=\"" << buildPath.string() << "/install\" " << additionalCmakeOptions;

        auto commandLine = oss.str();

        duk::log::info("Generating project with: '{}'", commandLine);

        int exitCode = std::system(commandLine.c_str());

        if (exitCode != 0) {
            throw std::runtime_error(fmt::format("failed to generate project, exit code: {}", exitCode));
        }
    }

    {
        std::ostringstream oss;
        oss << "cmake --build \"" << buildPath.string() << "\" --target install --config " << config;

        auto commandLine = oss.str();

        duk::log::info("Building project with: '{}'", commandLine);

        int exitCode = std::system(oss.str().c_str());

        if (exitCode != 0) {
            throw std::runtime_error(fmt::format("failed to build project, exit code: {}", exitCode));
        }
    }
}

void pack(Project* project) {
    if (project->root.empty()) {
        throw std::invalid_argument("project root is not initialized");
    }

    const auto projectPath = project->root / ".duk";
    const auto packPath = projectPath / "pack";
    const auto buildPath = projectPath / "builds/vs2022-pack";

    build(project, "Visual Studio 17 2022", buildPath, "Release", "-DDUK_PACK=ON");

    detail::pack_binaries(packPath, buildPath);

    detail::pack_resources(project, packPath);

    detail::pack_settings(project, packPath);

    duk::log::info("Completed packing project to '{}'", packPath.string());
}

}// namespace duk::project