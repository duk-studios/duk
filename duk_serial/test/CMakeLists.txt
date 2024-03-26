project(duk_serial_test)

set(DUK_OBJECTS_TEST_SOURCES
        main.cpp
)

add_executable(duk_serial_test ${DUK_OBJECTS_TEST_SOURCES})

target_include_directories(duk_serial_test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk_serial_test
        PRIVATE
        duk_serial
)

set_target_properties(duk_serial_test PROPERTIES CXX_STANDARD 20)
