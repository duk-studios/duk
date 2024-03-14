/// 14/03/2024
/// init_command.h

#ifndef DUK_CLI_INIT_COMMAND_H
#define DUK_CLI_INIT_COMMAND_H

#include <duk_cli/command_line.h>
#include <filesystem>

namespace duk::cli {

struct InitCommandCreateInfo {
    std::filesystem::path path;
};

class InitCommand : public Command {
public:
    explicit InitCommand(const InitCommandCreateInfo& initCommandCreateInfo);

    void execute() override;

private:
    std::filesystem::path m_path;
};

}

#endif // DUK_CLI_INIT_COMMAND_H

