/// 10/03/2024
/// resource_command.h

#ifndef DUK_CLI_RESOURCE_COMMAND_H
#define DUK_CLI_RESOURCE_COMMAND_H

#include <duk_cli/command_line.h>
#include <filesystem>

namespace duk::cli {

struct UpdateCommandCreateInfo {
    std::filesystem::path path;
};

class UpdateCommand : public Command {
public:
    explicit UpdateCommand(const UpdateCommandCreateInfo& initCommandCreateInfo);

    void execute() override;

private:
    std::filesystem::path m_path;
};

}// namespace duk::cli

#endif// DUK_CLI_RESOURCE_COMMAND_H
