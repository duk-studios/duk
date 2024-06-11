//
// Created by Ricardo on 10/06/2024.
//

#ifndef DUK_CLI_PACK_COMMAND_H
#define DUK_CLI_PACK_COMMAND_H

#include <duk_cli/command_line.h>
#include <filesystem>

namespace duk::cli {

struct PackCommandCreateInfo {
    std::filesystem::path path;
};

class PackCommand : public Command {
public:
    explicit PackCommand(const PackCommandCreateInfo& packCommandCreateInfo);

    void execute() override;

private:
    std::filesystem::path m_path;
};

}

#endif //DUK_CLI_PACK_COMMAND_H
