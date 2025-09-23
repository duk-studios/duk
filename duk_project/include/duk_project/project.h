/// 13/03/2024
/// project.h

#ifndef DUK_PROJECT_PROJECT_H
#define DUK_PROJECT_PROJECT_H

#include <duk_engine/settings.h>
#include <duk_project/resource.h>

#include <filesystem>
#include <unordered_map>

namespace duk::project {

struct ResourceEntry {
    std::filesystem::path dataFile;
    std::filesystem::path resourceFile;
};

struct Project {
    std::filesystem::path root;
    std::unordered_map<duk::resource::Id, ResourceEntry> resources;
    duk::engine::Settings settings;
};

void init(Project* project, std::filesystem::path path);

void open(Project* project, const std::filesystem::path &path);

void update(Project* project);

void build(Project* project, const std::string_view& generator, const std::string_view& folderName, const std::string_view& config, const std::string_view& additionalCmakeOptions = "");

void pack(Project* project, const std::filesystem::path& installedPath, const std::filesystem::path& packPath);

}// namespace duk::project

#endif// DUK_PROJECT_PROJECT_H
