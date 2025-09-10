project(duk_objects_test)

set(DUK_OBJECTS_TEST_SOURCES
        main.cpp
        )

add_executable(duk.objects.test ${DUK_OBJECTS_TEST_SOURCES})

target_include_directories(duk.objects.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk.objects.test
        PRIVATE
        duk::objects
        )
