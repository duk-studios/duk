//
// Created by sidao on 22/02/2024.
//
#include <duk_platform/win32/cursor_win_32.h>
#include <windows.h>

namespace duk::platform {

CursorWin32::CursorWin32() {
    m_cursorTypes[CursorType::ARROW] = LoadCursor(NULL, IDC_ARROW);
    m_cursorTypes[CursorType::DRAG] = LoadCursor(NULL, IDC_SIZEALL);
    m_cursorTypes[CursorType::TEXT] = LoadCursor(NULL, IDC_IBEAM);
    m_cursorTypes[CursorType::BUSY] = LoadCursor(NULL, IDC_WAIT);
    m_cursorTypes[CursorType::WORKING_BACKGROUND] = LoadCursor(NULL, IDC_APPSTARTING);
    m_cursorTypes[CursorType::UNAVAILABLE] = LoadCursor(NULL, IDC_NO);
}

void duk::platform::CursorWin32::show(bool visible) {
    ShowCursor(visible);
}

void CursorWin32::set_cursor(CursorType::Type cursorInfo) {
    if (m_currentCursor != cursorInfo) {
        SetCursor(m_cursorTypes[cursorInfo]);
        m_currentCursor = cursorInfo;
    }
}
}// namespace duk::platform