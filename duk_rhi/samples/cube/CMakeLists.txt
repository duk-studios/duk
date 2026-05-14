add_executable(duk.rhi.sample.cube main.cpp)

target_link_libraries(duk.rhi.sample.cube
        PRIVATE
        duk::rhi
        duk::platform
        duk::log
        unofficial::shaderc::shaderc
        )

