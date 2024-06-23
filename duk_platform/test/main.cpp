#include <duk_platform/window.h>
#include <iostream>

#include "duk_platform/win32/platform_win_32.h"

int main() {
    bool run = false;

    duk::platform::PlatformWin32CreateInfo platformWin32CreateInfo = {};
    platformWin32CreateInfo.instance = GetModuleHandle(NULL);

    duk::platform::PlatformWin32 platform(platformWin32CreateInfo);

    duk::event::Listener listener;

    duk::platform::WindowCreateInfo windowCreateInfo = {"MyWindow", 640, 720};

    std::cout << "Creating window" << std::endl;

    auto window = platform.create_window(windowCreateInfo);
    run = true;

    uint32_t myWindowHeight = window->height();
    uint32_t myWindowWidth = window->width();

    std::cout << "My window height: " << myWindowHeight << ", my window width: " << myWindowWidth << std::endl;

    listener.listen(window->window_close_event, [&window] {
        std::cout << "The window is closed!" << std::endl;
        window->close();
    });

    listener.listen(window->window_destroy_event, [&run]() {
        run = false;
    });

    listener.listen(window->window_resize_event, [](uint32_t width, uint32_t height) {
        std::cout << "My window height: " << height << ", my window width: " << width << std::endl;
    });

    listener.listen(window->mouse_movement_event, [](uint32_t width, uint32_t height) {
        std::cout << "My mouse x: " << height << ", my mouse y: " << width << std::endl;
    });

    listener.listen(window->mouse_button_event, [](duk::platform::MouseButton mouseButton, duk::platform::KeyAction action) {
        auto isPressAction = action == duk::platform::KeyAction::PRESS;

        switch (mouseButton) {
            case duk::platform::MouseButton::LEFT:
                isPressAction ? std::cout << "Left click was pressed!" << std::endl : std::cout << "Left click was released!" << std::endl;
                break;
            case duk::platform::MouseButton::RIGHT:
                isPressAction ? std::cout << "Right click was pressed!" << std::endl : std::cout << "Right click was released!" << std::endl;
                break;
            case duk::platform::MouseButton::MIDDLE:
                isPressAction ? std::cout << "Middle click was pressed!" << std::endl : std::cout << "Middle click was released!" << std::endl;
                break;
        }
    });

    listener.listen(window->mouse_wheel_movement_event, [](uint32_t fwKeys, uint32_t zDelta) {
        std::cout << "My mouse wheel fwKeys: " << fwKeys << ", my mouse wheel zDelta: " << zDelta << std::endl;
    });

    listener.listen(window->key_event, [](duk::platform::Keys key, duk::platform::KeyModifiers::Mask mods, duk::platform::KeyAction action) {
        std::cout << "I pressed the key: " << static_cast<uint32_t>(key) << std::endl;
    });

    window->show();

    while (run) {
        platform.pool_events();
    }

    return 0;
}
