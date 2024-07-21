//
// Created by Ricardo on 24/06/2024.
//

#include <duk_platform/win32/console_win_32.h>

#include <iostream>

#include <Windows.h>

namespace duk::platform {

namespace detail {

static void enable_terminal_mode(DWORD stdHandle, DWORD mode) {
    HANDLE hOut = GetStdHandle(stdHandle);

    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);

    dwMode |= mode;
    SetConsoleMode(hOut, dwMode);
}

static void disable_terminal_mode(DWORD stdHandle, DWORD mode) {
    HANDLE hOut = GetStdHandle(stdHandle);

    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);

    dwMode &= ~mode;
    SetConsoleMode(hOut, dwMode);
}

static void enable_virtual_terminal() {
    enable_terminal_mode(STD_OUTPUT_HANDLE, ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    enable_terminal_mode(STD_INPUT_HANDLE, ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    enable_terminal_mode(STD_ERROR_HANDLE, ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

static void disable_virtual_terminal() {
    disable_terminal_mode(STD_OUTPUT_HANDLE, ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    disable_terminal_mode(STD_INPUT_HANDLE, ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    disable_terminal_mode(STD_ERROR_HANDLE, ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

}// namespace detail

ConsoleWin32::ConsoleWin32()
    : m_fOut(nullptr)
    , m_fErr(nullptr)
    , m_fIn(nullptr)
    , m_open(GetStdHandle(STD_OUTPUT_HANDLE) && GetStdHandle(STD_INPUT_HANDLE) && GetStdHandle(STD_ERROR_HANDLE)) {
    if (m_open) {
        detail::enable_virtual_terminal();
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

    detail::enable_virtual_terminal();

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

    detail::enable_virtual_terminal();

    return true;
}

void ConsoleWin32::close() {
    if (!m_open) {
        return;
    }

    detail::disable_virtual_terminal();

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
