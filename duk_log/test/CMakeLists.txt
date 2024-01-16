project(duk_log_test)

set(DUK_LOG_TEST_SOURCES
        main.cpp
        )

add_executable(duk_log_test ${DUK_LOG_TEST_SOURCES})

target_include_directories(duk_log_test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_log_test
        PRIVATE
        duk_log
        )

set_target_properties(duk_log_test PROPERTIES CXX_STANDARD 20)
