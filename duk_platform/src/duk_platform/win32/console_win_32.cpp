//
// Created by Ricardo on 24/06/2024.
//

#include <duk_platform/win32/console_win_32.h>

#include <iostream>

#include <Windows.h>

namespace duk::platform {

ConsoleWin32::ConsoleWin32()
    : m_fOut(nullptr)
    , m_fErr(nullptr)
    , m_fIn(nullptr)
    , m_open(GetConsoleWindow() != nullptr) {
    if (m_open) {
        open_streams();
    }
}

ConsoleWin32::~ConsoleWin32() {
    close();
}

bool ConsoleWin32::attach() {
    if (m_open) {
        return false;
    }

    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        return false;
    }

    open_streams();

    m_open = true;

    return true;
}

bool ConsoleWin32::open() {
    if (m_open) {
        return false;
    }

    if (!AllocConsole()) {
        return false;
    }

    open_streams();

    m_open = true;

    return true;
}

void ConsoleWin32::close() {
    if (!m_open) {
        return;
    }
    close_streams();

    FreeConsole();

    m_open = false;
}

std::ostream& ConsoleWin32::out() {
    return std::cout;
}

std::ostream& ConsoleWin32::err() {
    return std::cerr;
}

std::istream& ConsoleWin32::in() {
    return std::cin;
}

bool ConsoleWin32::is_open() const {
    return m_open;
}

void ConsoleWin32::open_streams() {
    freopen_s(&m_fOut, "CONOUT$", "w", stdout);
    freopen_s(&m_fErr, "CONOUT$", "w", stderr);
    freopen_s(&m_fIn, "CONIN$", "r", stdin);
}

void ConsoleWin32::close_streams() {
    if (m_fOut) {
        fclose(m_fOut);
        m_fOut = nullptr;
    }
    if (m_fErr) {
        fclose(m_fErr);
        m_fErr = nullptr;
    }
    if (m_fIn) {
        fclose(m_fIn);
        m_fIn = nullptr;
    }
}

}// namespace duk::platform
