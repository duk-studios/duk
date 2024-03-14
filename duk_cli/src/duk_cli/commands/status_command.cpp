/// 14/03/2024
/// status_command.cpp

#include <duk_cli/commands/status_command.h>

#include <duk_project/project.h>
#include <duk_log/log.h>

namespace duk::cli {

StatusCommand::StatusCommand(const StatusCommandCreateInfo& statusCommandCreateInfo) :
    m_path(statusCommandCreateInfo.path) {

}

void StatusCommand::execute() {
    duk::project::Project project = {};
    duk::project::open(&project, m_path);

    duk::project::resource_scan(&project);
    duk::log::info("Resources:");
    for (auto& [id, resource] : project.resources) {
        duk::log::info("  id: {} - file: {}", id.value(), resource.resourceFile.string());
    }

    duk::log::info("Settings:");
    duk::log::info("   name: {}", project.settings.name);
    duk::log::info("   resolution: {}x{}", project.settings.resolution.x, project.settings.resolution.y);
    duk::log::info("   initial scene id: {}", project.settings.scene.value());
}

}