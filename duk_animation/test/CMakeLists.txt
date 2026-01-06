
set(DUK_ANIMATION_TEST_SOURCES
        property.cc
)

add_executable(duk.animation.test ${DUK_ANIMATION_TEST_SOURCES})

target_include_directories(duk.animation.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk.animation.test
        PRIVATE
        duk::animation
        Catch2::Catch2WithMain
)

catch_discover_tests(duk.animation.test)