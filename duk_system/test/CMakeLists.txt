project(duk_system_test)

set(DUK_SYSTEM_TEST_SOURCES
        main.cpp
)

add_executable(duk_system_test ${DUK_SYSTEM_TEST_SOURCES})

target_include_directories(duk_system_test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk_system_test
        PRIVATE
        duk_system
)

set_target_properties(duk_system_test PROPERTIES CXX_STANDARD 20)
