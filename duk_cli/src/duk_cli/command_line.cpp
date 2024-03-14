//
// Created by rov on 12/15/2023.
//

// duk_cli
#include <duk_cli/command_line.h>
#include <duk_cli/commands/init_command.h>
#include <duk_cli/commands/status_command.h>
#include <duk_cli/commands/update_command.h>

// std
#include <filesystem>

namespace duk::cli {

namespace detail {

std::unique_ptr<Command> make_init_command() {
    InitCommandCreateInfo resourceCommandCreateInfo = {};
    resourceCommandCreateInfo.path = std::filesystem::current_path();

    return std::make_unique<InitCommand>(resourceCommandCreateInfo);
}

std::unique_ptr<Command> make_update_command() {
    UpdateCommandCreateInfo updateCommandCreateInfo = {};
    updateCommandCreateInfo.path = std::filesystem::current_path();

    return std::make_unique<UpdateCommand>(updateCommandCreateInfo);
}

std::unique_ptr<Command> make_status_command() {
    StatusCommandCreateInfo statusCommandCreateInfo = {};
    statusCommandCreateInfo.path = std::filesystem::current_path();

    return std::make_unique<StatusCommand>(statusCommandCreateInfo);
}

}// namespace detail

Command::~Command() = default;

CommandLine::CommandLine(int argc, const char* argv[]) {
    if (argc < 1) {
        throw std::invalid_argument("insufficient arguments");
    }

    std::string commandName = argv[0];

    if (commandName == "init") {
        m_command = detail::make_init_command();
        return;
    }
    if (commandName == "update") {
        m_command = detail::make_update_command();
        return;
    }
    if (commandName == "status") {
        m_command = detail::make_status_command();
        return;
    }

    throw std::invalid_argument("unknown command: " + commandName);
}

Command* CommandLine::command() const {
    return m_command.get();
}

}// namespace duk::cli
