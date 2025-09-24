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
    std::filesystem::path packPath;
    std::vector<std::string> packFiles;
};

class PackCommand : public Command {
public:
    explicit PackCommand(const PackCommandCreateInfo& packCommandCreateInfo);

    void execute() override;

private:
    std::filesystem::path m_projectPath;
    std::filesystem::path m_packPath;
    std::vector<std::string> m_packFiles;
};

}// namespace duk::cli

#endif//DUK_CLI_PACK_COMMAND_H
