/// 04/03/2024
/// resource.h

#ifndef DUK_PROJECT_RESOURCE_H
#define DUK_PROJECT_RESOURCE_H

#include <duk_resource/resource.h>

#include <filesystem>
#include <string>

#include <duk_resource/file.h>

namespace duk::project {

struct Project;

duk::resource::ResourceFile load_resource_file(const std::filesystem::path& path);

/// scans the project and updates tracked resources
/// returns a set with untracked resources
std::set<std::filesystem::path> resource_scan(Project* project);

/// creates a resource file and returns its id
duk::resource::Id resource_track(Project* project, const std::filesystem::path& resource);

}// namespace duk::project

#endif// DUK_PROJECT_RESOURCE_H
