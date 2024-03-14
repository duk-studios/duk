/// 04/03/2024
/// resource.h

#ifndef DUK_PROJECT_RESOURCE_H
#define DUK_PROJECT_RESOURCE_H

#include <duk_resource/resource.h>

#include <filesystem>
#include <string>

namespace duk::project {

struct Project;

/// scans the project and updates tracked resources
/// returns a set with untracked resources
std::set<std::filesystem::path> resource_scan(Project* project);

void resource_track(Project* project, const std::filesystem::path& resource);

}// namespace duk::project

#endif// DUK_PROJECT_RESOURCE_H
