add_executable(duk.rhi.sample.axes main.cpp)

target_link_libraries(duk.rhi.sample.axes
        PRIVATE
        duk::rhi
        duk::platform
        duk::log
        unofficial::shaderc::shaderc
        )

