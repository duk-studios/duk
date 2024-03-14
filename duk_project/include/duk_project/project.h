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
    std::filesystem::path resourceFile;
    std::filesystem::path trackFile;
};

struct Project {
    std::filesystem::path root;
    std::unordered_map<duk::resource::Id, ResourceEntry> resources;
    duk::engine::Settings settings;
};

void init(Project* project, std::filesystem::path path);

void open(Project* project, std::filesystem::path path);

void update(Project* project);

}

#endif // DUK_PROJECT_PROJECT_H

