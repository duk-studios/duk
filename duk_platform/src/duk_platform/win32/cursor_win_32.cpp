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

    SetCursor(m_cursorTypes[CursorType::ARROW]);
}

void CursorWin32::show(bool visible) {
    m_visible = visible;
    SetCursor(current_handle());
}

void CursorWin32::set_type(CursorType type) {
    if (m_currentCursorType != type) {
        m_currentCursorType = type;
        if (m_visible) {
            SetCursor(m_cursorTypes[type]);
        }
    }
}

HCURSOR CursorWin32::current_handle() const {
    if (!m_visible) {
        return nullptr;
    }
    auto it = m_cursorTypes.find(m_currentCursorType);
    if (it != m_cursorTypes.end()) {
        return it->second;
    }
    return nullptr;
}

}// namespace duk::platform