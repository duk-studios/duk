add_executable(duk.rhi.sample.dynamic main.cpp)

target_link_libraries(duk.rhi.sample.dynamic
        PRIVATE
        duk::rhi
        duk::platform
        duk::log
        )
