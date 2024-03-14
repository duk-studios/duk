/// 14/03/2024
/// status_command.h

#ifndef DUK_CLI_STATUS_COMMAND_H
#define DUK_CLI_STATUS_COMMAND_H

#include <duk_cli/command_line.h>
#include <filesystem>

namespace duk::cli {

struct StatusCommandCreateInfo {
    std::filesystem::path path;
};

class StatusCommand : public Command {
public:
    explicit StatusCommand(const StatusCommandCreateInfo& statusCommandCreateInfo);

    void execute() override;

private:
    std::filesystem::path m_path;
};

}// namespace duk::cli

#endif// DUK_CLI_STATUS_COMMAND_H
