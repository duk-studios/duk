//
// Created by Ricardo on 10/06/2024.
//

#include <duk_cli/commands/pack_command.h>

#include <duk_project/project.h>

namespace duk::cli {

PackCommand::PackCommand(const PackCommandCreateInfo& packCommandCreateInfo)
    : m_path(packCommandCreateInfo.path) {
}

void PackCommand::execute() {
    duk::project::Project project = {};

    duk::project::open(&project, m_path);

    duk::project::pack(&project);
}

}
