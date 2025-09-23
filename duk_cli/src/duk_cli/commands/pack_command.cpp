//
// Created by Ricardo on 10/06/2024.
//

#include <duk_cli/commands/pack_command.h>

#include <duk_project/project.h>

namespace duk::cli {

PackCommand::PackCommand(const PackCommandCreateInfo& packCommandCreateInfo)
    : m_projectPath(packCommandCreateInfo.projectPath)
    , m_installPath(packCommandCreateInfo.installPath)
    , m_packPath(packCommandCreateInfo.packPath) {
}

void PackCommand::execute() {
    duk::project::Project project = {};

    duk::project::open(&project, m_projectPath);

    duk::project::pack(&project, m_installPath, m_packPath);
}

}// namespace duk::cli
