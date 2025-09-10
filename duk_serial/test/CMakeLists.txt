
set(DUK_OBJECTS_TEST_SOURCES
        main.cpp
)

add_executable(duk.serial.test ${DUK_OBJECTS_TEST_SOURCES})

target_include_directories(duk.serial.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk.serial.test
        PRIVATE
        duk::serial
)
