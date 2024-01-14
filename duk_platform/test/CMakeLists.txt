project(duk_platform_test)

set(DUK_PLATFORM_TEST_SOURCES
        main.cpp
        )

add_executable(duk_platform_test ${DUK_PLATFORM_TEST_SOURCES})

target_include_directories(duk_platform_test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_platform_test
        PRIVATE
        duk_platform
        )

set_target_properties(duk_platform_test PROPERTIES CXX_STANDARD 20)
