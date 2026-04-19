#include <duk_platform/window.h>
#include <duk_platform/platform.h>
#include <iostream>

int main() {
    volatile bool run = false;

    auto platform = duk::platform::create_default_platform();

    duk::event::Listener listener;

    duk::platform::WindowCreateInfo windowCreateInfo = {"MyWindow", 640, 720};
    windowCreateInfo.style = duk::platform::WindowStyle::FULLSCREEN;

    std::cout << "Creating window" << std::endl;

    auto window = platform->create_window(windowCreateInfo);
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
        std::cout << "Window resize width: " << width << ", height: " << height << std::endl;
    });

    listener.listen(window->mouse_movement_event, [](int32_t width, int32_t height) {
        std::cout << "Mouse move x: " << height << ", y: " << width << std::endl;
    });

    listener.listen(window->mouse_enter_event, [](int32_t width, int32_t height) {
        std::cout << "Mouse enter x: " << height << ", y: " << width << std::endl;
    });

    listener.listen(window->mouse_leave_event, [](int32_t width, int32_t height) {
        std::cout << "Mouse leave x: " << height << ", y: " << width << std::endl;
    });

    listener.listen(window->mouse_button_event, [](duk::platform::MouseButton mouseButton, duk::platform::KeyAction action) {
        auto isPressAction = action == duk::platform::KeyAction::PRESS;

        switch (mouseButton) {
            case duk::platform::MouseButton::LEFT:
                isPressAction ? std::cout << "Left was pressed!" << std::endl : std::cout << "Left was released!" << std::endl;
                break;
            case duk::platform::MouseButton::RIGHT:
                isPressAction ? std::cout << "Right was pressed!" << std::endl : std::cout << "Right was released!" << std::endl;
                break;
            case duk::platform::MouseButton::MIDDLE:
                isPressAction ? std::cout << "Middle was pressed!" << std::endl : std::cout << "Middle was released!" << std::endl;
                break;
        }
    });

    listener.listen(window->mouse_wheel_movement_event, [](uint32_t fwKeys, int16_t zDelta) {
        std::cout << "My mouse wheel fwKeys: " << fwKeys << ", my mouse wheel zDelta: " << zDelta << std::endl;
    });

    listener.listen(window->key_event, [window](duk::platform::Keys key, duk::platform::KeyModifiers::Mask mods, duk::platform::KeyAction action) {
        std::cout << "I pressed the key: " << static_cast<uint32_t>(key) << std::endl;
        if (key == duk::platform::Keys::H) {
            window->hide();
        }
        if (key == duk::platform::Keys::ESC) {
            window->close();
        }
    });

    window->show();

    while (run) {
        platform->pool_events();
    }

    return 0;
}
