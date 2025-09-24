//
// Created by rov on 12/15/2023.
//

// duk_cli
#include <duk_cli/command_line.h>
#include <duk_cli/commands/init_command.h>
#include <duk_cli/commands/status_command.h>
#include <duk_cli/commands/update_command.h>
#include <duk_cli/commands/pack_command.h>
#include <duk_log/log.h>

#define CXXOPTS_VECTOR_DELIMITER ';'
#include <cxxopts.hpp>

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

std::unique_ptr<Command> make_pack_command(const std::filesystem::path& projectPath, const std::filesystem::path& packPath, const std::vector<std::string>& packFiles) {
    PackCommandCreateInfo packCommandCreateInfo = {};
    packCommandCreateInfo.projectPath = projectPath;
    packCommandCreateInfo.packFiles = packFiles;
    packCommandCreateInfo.packPath = packPath;

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
        ("command", "Main command", cxxopts::value<std::string>())
        ("f,files", "Files to pack", cxxopts::value<std::vector<std::string>>())
        ("p,project", "Path to project directory", cxxopts::value<std::string>())
        ("o,output", "Path to pack directory", cxxopts::value<std::string>())
        ("h,help", "Print usage");
    // clang-format on

    options.parse_positional("command");
    options.positional_help("command [--project path --output path --files [file1;file2;dir1;...]]");

    const auto result = options.parse(argc, argv);

    if (result.count("help")) {
        duk::log::info(options.help());
        return;
    }

    const auto commandName = result["command"].as<std::string>();

    const auto projectPath = result.count("project") > 0 ? std::filesystem::path(result["project"].as<std::string>()) : std::filesystem::current_path();

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
        const auto packFiles = result["files"].as<std::vector<std::string>>();
        const auto packPath = result["output"].as<std::string>();
        m_command = detail::make_pack_command(projectPath, packPath, packFiles);
        return;
    }

    throw std::invalid_argument("unknown command: " + commandName);
}

Command* CommandLine::command() const {
    return m_command.get();
}

}// namespace duk::cli
