add_executable(duk.rhi.sample.clear main.cpp)

target_link_libraries(duk.rhi.sample.clear
        PRIVATE
        duk::rhi
        duk::platform
        duk::log
        )

