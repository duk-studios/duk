add_executable(duk.platform.sample.fullscreen main.cpp)

target_link_libraries(duk.platform.sample.fullscreen
        PRIVATE
        duk::platform
        )

