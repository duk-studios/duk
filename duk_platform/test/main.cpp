#include <iostream>
#include <duk_platform/window.h>

int main() {
    bool run = false;
     
    duk::events::EventListener listener;
    
    std::shared_ptr<duk::platform::Window> window;
    
    duk::platform::WindowCreateInfo windowCreateInfo = {"MyWindow", 640, 720};
    
    {
        auto result = duk::platform::Window::create_window(windowCreateInfo);
        std::cout<< "Creating window" << std::endl;    
        if (!result) {
            throw std::runtime_error("failed to create engine window: " + result.error().description());
        }

        std::cout<< "Window creation succeed!" << std::endl;
        window = std::move(result.value());
        run = true;
    }

    uint32_t myWindowHeight = window->height();
    uint32_t myWindowWidth = window->width();

    std::cout<< "My window height: " << myWindowHeight << ", my window width: " << myWindowWidth << std::endl;

    listener.listen(window->window_close_event, [&window] {
        std::cout<<"The window is closed!" << std::endl;
        window->close();
    });

    listener.listen(window->window_destroy_event, [&run](){
        run = false;
    });

    listener.listen(window->window_resize_event, [](uint32_t width, uint32_t height) {
        std::cout<< "My window height: " << height << ", my window width: " << width << std::endl;
    });

    listener.listen(window->mouse_movement_event, [](uint32_t width, uint32_t height) {
        std::cout<< "My mouse x: " << height << ", my mouse y: " << width << std::endl;
    });

    listener.listen(window->mouse_button_event, [](duk::platform::MouseButton mouseButton, duk::platform::KeyAction action) {
        auto isPressAction = action == duk::platform::KeyAction::PRESS;
        
        switch (mouseButton) {
        case duk::platform::MouseButton::LEFT:
                 isPressAction ? std::cout<< "Left click was pressed!" << std::endl : std::cout<< "Left click was released!" << std::endl;
                break;
            case duk::platform::MouseButton::RIGHT:
                isPressAction ? std::cout<< "Right click was pressed!" << std::endl : std::cout<< "Right click was released!" << std::endl;
                break;
            case duk::platform::MouseButton::MIDDLE:
                isPressAction ? std::cout<< "Middle click was pressed!" << std::endl : std::cout<< "Middle click was released!" << std::endl;
                break;
        }
    });

    listener.listen(window->mouse_wheel_movement_event, [](uint32_t fwKeys, uint32_t zDelta){
        std::cout<< "My mouse wheel fwKeys: " << fwKeys << ", my mouse wheel zDelta: " << zDelta << std::endl;
    });
    
    listener.listen(window->key_event, [](duk::platform::Keys key, duk::platform::KeyModifiers::Mask mods, duk::platform::KeyAction action){
      
        std::cout<< "I pressed the key: " << static_cast<uint32_t>(key) << std::endl;
    });
    
    window->show();
    
    while (run) {
        window->pool_events();
    }
    
    return 0;
}
