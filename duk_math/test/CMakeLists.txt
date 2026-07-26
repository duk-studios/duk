
set(DUK_MATH_TEST_SOURCES
        glm.cc
        aabb.cc
        frustum.cc
)

add_executable(duk.math.test ${DUK_MATH_TEST_SOURCES})

target_include_directories(duk.math.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk.math.test
        PRIVATE
        duk::math
        Catch2::Catch2WithMain
)

catch_discover_tests(duk.math.test)