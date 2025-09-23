//
// Created by rov on 12/15/2023.
//

// duk_cli
#include <duk_cli/command_line.h>
#include <duk_cli/commands/init_command.h>
#include <duk_cli/commands/status_command.h>
#include <duk_cli/commands/update_command.h>
#include <duk_cli/commands/pack_command.h>

// std
#include <cxxopts.hpp>
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

std::unique_ptr<Command> make_pack_command() {
    PackCommandCreateInfo packCommandCreateInfo = {};
    packCommandCreateInfo.path = std::filesystem::current_path();

    return std::make_unique<PackCommand>(packCommandCreateInfo);
}

std::unique_ptr<Command> make_status_command() {
    StatusCommandCreateInfo statusCommandCreateInfo = {};
    statusCommandCreateInfo.path = std::filesystem::current_path();

    return std::make_unique<StatusCommand>(statusCommandCreateInfo);
}

}// namespace detail

Command::~Command() = default;

CommandLine::CommandLine(int argc, const char* argv[]) {

    // clang-format off
    cxxopts::Options options("duk");
    options.add_options()
        ("command", "Command to execute (init, update, status, pack)")
        ("i,install", "Path to install directory")
        ("p,project", "Path to project directory")
        ("o,output", "Path to pack directory");
    // clang-format on

    options.parse_positional("command");

    const auto result = options.parse(argc, argv);

    const auto commandName = result["command"].as<std::string>();

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
    if (commandName == "pack") {
        m_command = detail::make_pack_command();
        return;
    }

    throw std::invalid_argument("unknown command: " + commandName);
}

Command* CommandLine::command() const {
    return m_command.get();
}

}// namespace duk::cli
