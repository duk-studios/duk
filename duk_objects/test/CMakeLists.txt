project(duk_objects_test)

set(DUK_OBJECTS_TEST_SOURCES
        main.cpp
        )

add_executable(duk_objects_test ${DUK_OBJECTS_TEST_SOURCES})

target_include_directories(duk_objects_test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_objects_test
        PRIVATE
        duk_objects
        )

set_target_properties(duk_objects_test PROPERTIES CXX_STANDARD 20)
