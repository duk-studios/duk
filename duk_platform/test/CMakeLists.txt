project(duk_platform_test)

set(DUK_PLATFORM_TEST_SOURCES
        main.cpp
        )

add_executable(duk.platform.test ${DUK_PLATFORM_TEST_SOURCES})

target_include_directories(duk.platform.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk.platform.test
        PRIVATE
        duk::platform
        )

