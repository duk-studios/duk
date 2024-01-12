#include <iostream>
#include "duk_platform/window.h"

int main() {
    bool m_run = false;
    
    duk::events::EventListener m_listener;
    
    std::shared_ptr<duk::platform::Window> m_myWindow;
    
    duk::platform::WindowCreateInfo windowCreateInfo = {"MyWindow", 640, 720};
    
    {
        auto result = duk::platform::Window::create_window(windowCreateInfo);
        std::cout<< "Creating window" << std::endl;    
        if (!result) {
            throw std::runtime_error("failed to create engine window: " + result.error().description());
        }

        std::cout<< "Window creation succeed!" << std::endl;
        m_myWindow = std::move(result.value());
        m_run = true;
    }

    uint32_t myWindowHeight = m_myWindow->height();
    uint32_t myWindowWidth = m_myWindow->width();

    std::cout<< "My window height: " << myWindowHeight << ", my window width: " << myWindowWidth << std::endl;

    m_listener.listen(m_myWindow->window_close_event, [&m_myWindow] {
        std::cout<<"The window is closed!" << std::endl;
        m_myWindow->close();
    });

    m_listener.listen(m_myWindow->window_destroy_event, [&m_run](){
        m_run = false;
    });

    m_listener.listen(m_myWindow->window_resize_event, [](uint32_t width, uint32_t height) {
        std::cout<< "My window height: " << height << ", my window width: " << width << std::endl;
    });

    m_myWindow->show();
    
    while (m_run) {
        m_myWindow->pool_events();
    }
    
    return 0;
}
