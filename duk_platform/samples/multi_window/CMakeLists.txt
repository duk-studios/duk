add_executable(duk.platform.sample.multi_window main.cpp)

target_link_libraries(duk.platform.sample.multi_window
        PRIVATE
        duk::platform
        )

