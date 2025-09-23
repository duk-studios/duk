//
// Created by Ricardo on 10/06/2024.
//

#ifndef DUK_CLI_PACK_COMMAND_H
#define DUK_CLI_PACK_COMMAND_H

#include <duk_cli/command_line.h>
#include <filesystem>

namespace duk::cli {

struct PackCommandCreateInfo {
    std::filesystem::path projectPath;
    std::filesystem::path installPath;
    std::filesystem::path packPath;
};

class PackCommand : public Command {
public:
    explicit PackCommand(const PackCommandCreateInfo& packCommandCreateInfo);

    void execute() override;

private:
    std::filesystem::path m_projectPath;
    std::filesystem::path m_installPath;
    std::filesystem::path m_packPath;
};

}// namespace duk::cli

#endif//DUK_CLI_PACK_COMMAND_H
