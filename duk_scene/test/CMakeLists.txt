project(duk_scene_test)

set(DUK_SCENE_TEST_SOURCES
        main.cpp
        )

add_executable(duk_scene_test ${DUK_SCENE_TEST_SOURCES})

target_include_directories(duk_scene_test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_scene_test
        PRIVATE
        duk_scene
        )

set_target_properties(duk_scene_test PROPERTIES CXX_STANDARD 20)
