
set(DUK_OBJECTS_TEST_SOURCES
        json.cc
)

add_executable(duk.serial.test ${DUK_OBJECTS_TEST_SOURCES})

target_include_directories(duk.serial.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk.serial.test
        PRIVATE
        duk::serial
        Catch2::Catch2WithMain
)

catch_discover_tests(duk.serial.test)