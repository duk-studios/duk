add_executable(duk.rhi.sample.triangle main.cpp)

target_link_libraries(duk.rhi.sample.triangle
        PRIVATE
        duk::rhi
        duk::platform
        duk::log
        )

