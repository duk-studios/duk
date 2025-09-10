project(duk_system_test)

set(DUK_SYSTEM_TEST_SOURCES
        main.cpp
)

add_executable(duk.system.test ${DUK_SYSTEM_TEST_SOURCES})

target_include_directories(duk.system.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk.system.test
        PRIVATE
        duk::system
)
