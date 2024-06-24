//
// Created by Ricardo on 24/06/2024.
//

#ifndef DUK_PLATFORM_CONSOLE_WIN_32_H
#define DUK_PLATFORM_CONSOLE_WIN_32_H

#include <duk_platform/console.h>

namespace duk::platform {

class ConsoleWin32 final : public Console {
public:
    ConsoleWin32();

    ~ConsoleWin32() override;

    bool attach() override;

    bool open() override;

    void close() override;

    std::ostream& out() override;

    std::ostream& err() override;

    std::istream& in() override;

    DUK_NO_DISCARD bool is_open() const override;

private:
    void open_streams();

    void close_streams();

private:
    FILE* m_fOut;
    FILE* m_fErr;
    FILE* m_fIn;
    bool m_open;
};

}// namespace duk::platform

#endif//DUK_PLATFORM_CONSOLE_WIN_32_H
