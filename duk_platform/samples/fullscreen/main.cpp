//
// duk_platform sample: single fullscreen window with cursor and input handling
//

#include <duk_platform/cursor.h>
#include <duk_platform/platform.h>

#include <iostream>

int main() {
    volatile bool run = true;

    auto platform = duk::platform::create_platform();

    duk::event::Listener listener;

    duk::platform::WindowCreateInfo createInfo = {"Fullscreen", 1920, 1080, duk::platform::WindowStyle::FULLSCREEN};

    auto window = platform->create_window(createInfo);

    listener.listen(window->window_close_event, [&window] {
        window->close();
    });

    listener.listen(window->window_destroy_event, [&run] {
        run = false;
    });

    listener.listen(window->window_resize_event, [](uint32_t width, uint32_t height) {
        std::cout << "resize: " << width << "x" << height << std::endl;
    });

    listener.listen(window->mouse_button_event, [&window](duk::platform::MouseButton button, duk::platform::KeyAction action) {
        if (button == duk::platform::MouseButton::LEFT && action == duk::platform::KeyAction::PRESS) {
            std::cout << "left click — setting DRAG cursor" << std::endl;
            window->cursor()->set_type(duk::platform::CursorType::DRAG);
        }
        if (button == duk::platform::MouseButton::LEFT && action == duk::platform::KeyAction::RELEASE) {
            std::cout << "left release — setting ARROW cursor" << std::endl;
            window->cursor()->set_type(duk::platform::CursorType::ARROW);
        }
    });

    listener.listen(window->key_event, [&window](duk::platform::Keys key, duk::platform::KeyModifiers::Mask mods, duk::platform::KeyAction action) {
        if (action != duk::platform::KeyAction::PRESS) {
            return;
        }
        std::cout << "key: " << static_cast<uint32_t>(key) << std::endl;
        if (key == duk::platform::Keys::ESC) {
            window->close();
        }
    });

    window->show();

    std::cout << "Fullscreen window created. Press ESC to exit." << std::endl;

    while (run) {
        platform->pool_events();
    }

    return 0;
}
