/// 14/03/2024
/// init_command.cpp

#include <duk_cli/commands/init_command.h>

#include <duk_project/project.h>

namespace duk::cli {

InitCommand::InitCommand(const InitCommandCreateInfo& initCommandCreateInfo)
    : m_path(initCommandCreateInfo.path) {
}

void InitCommand::execute() {
    duk::project::Project project = {};
    duk::project::init(&project, m_path);
}

}// namespace duk::cli