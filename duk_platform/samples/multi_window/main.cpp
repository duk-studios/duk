//
// duk_platform sample: two non-fullscreen windows with cursor type testing
//
// Controls (per-window, applies to the window that has focus):
//   1 — ARROW cursor
//   2 — DRAG cursor
//   3 — TEXT cursor
//   4 — BUSY cursor
//   5 — WORKING_BACKGROUND cursor
//   6 — UNAVAILABLE cursor
//   H — hide cursor
//   S — show cursor
//   ESC — close window and exit
//

#include <duk_platform/cursor.h>
#include <duk_platform/platform.h>

#include <array>
#include <iostream>

static const std::array<std::pair<duk::platform::CursorType, const char*>, 6> s_cursorTypes = {{
        {duk::platform::CursorType::ARROW, "ARROW"},
        {duk::platform::CursorType::DRAG, "DRAG"},
        {duk::platform::CursorType::TEXT, "TEXT"},
        {duk::platform::CursorType::BUSY, "BUSY"},
        {duk::platform::CursorType::WORKING_BACKGROUND, "WORKING_BACKGROUND"},
        {duk::platform::CursorType::UNAVAILABLE, "UNAVAILABLE"},
}};

static void setup_window_events(const std::shared_ptr<duk::platform::Window>& window, const std::string& name, volatile bool& run) {
    static duk::event::Listener listener;

    listener.listen(window->window_close_event, [&window] {
        window->close();
    });

    listener.listen(window->window_destroy_event, [&run] {
        run = false;
    });

    listener.listen(window->window_resize_event, [name](uint32_t width, uint32_t height) {
        std::cout << "[" << name << "] resize: " << width << "x" << height << std::endl;
    });

    listener.listen(window->mouse_enter_event, [name](int32_t x, int32_t y) {
        std::cout << "[" << name << "] mouse enter" << std::endl;
    });

    listener.listen(window->mouse_leave_event, [name](int32_t x, int32_t y) {
        std::cout << "[" << name << "] mouse leave" << std::endl;
    });

    listener.listen(window->key_event, [name, &window](duk::platform::Keys key, duk::platform::KeyModifiers::Mask mods, duk::platform::KeyAction action) {
        if (action != duk::platform::KeyAction::PRESS) {
            return;
        }

        // Number keys 1–6 set cursor type
        if (key >= duk::platform::Keys::NUM_1 && key <= duk::platform::Keys::NUM_6) {
            auto index = static_cast<int>(key) - static_cast<int>(duk::platform::Keys::NUM_1);
            const auto& [cursorType, cursorName] = s_cursorTypes[index];
            window->cursor()->set_type(cursorType);
            std::cout << "[" << name << "] cursor set to " << cursorName << std::endl;
            return;
        }

        switch (key) {
            case duk::platform::Keys::H:
                window->cursor()->show(false);
                std::cout << "[" << name << "] cursor hidden" << std::endl;
                break;
            case duk::platform::Keys::S:
                window->cursor()->show(true);
                std::cout << "[" << name << "] cursor shown" << std::endl;
                break;
            case duk::platform::Keys::ESC:
                window->close();
                break;
            default:
                break;
        }
    });
}

int main() {
    volatile bool run = true;

    auto platform = duk::platform::create_platform();

    duk::platform::WindowCreateInfo createInfoA = {"Window A", 640, 480, duk::platform::WindowStyle::STANDARD};
    duk::platform::WindowCreateInfo createInfoB = {"Window B", 640, 480, duk::platform::WindowStyle::STANDARD};

    auto windowA = platform->create_window(createInfoA);
    auto windowB = platform->create_window(createInfoB);

    setup_window_events(windowA, "A", run);
    setup_window_events(windowB, "B", run);

    windowA->show();
    windowB->show();

    std::cout << "Two windows created." << std::endl;
    std::cout << "  1-6: set cursor type (ARROW, DRAG, TEXT, BUSY, WORKING_BACKGROUND, UNAVAILABLE)" << std::endl;
    std::cout << "  H: hide cursor  |  S: show cursor  |  ESC: exit" << std::endl;

    while (run) {
        platform->pool_events();
    }

    return 0;
}
