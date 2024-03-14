/// 10/03/2024
/// resource_command.cpp

// duk_cli
#include <duk_cli/commands/update_command.h>

// duk
#include <duk_project/project.h>

namespace duk::cli {

UpdateCommand::UpdateCommand(const UpdateCommandCreateInfo& initCommandCreateInfo)
    : m_path(initCommandCreateInfo.path) {

}

void UpdateCommand::execute() {
    duk::project::Project project = {};

    duk::project::open(&project, m_path);

    duk::project::update(&project);
}

}