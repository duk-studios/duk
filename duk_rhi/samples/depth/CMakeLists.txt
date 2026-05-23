add_executable(duk.rhi.sample.depth main.cpp)

target_link_libraries(duk.rhi.sample.depth
        PRIVATE
        duk::rhi
        duk::platform
        duk::log
        )

