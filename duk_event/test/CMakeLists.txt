
set(DUK_EVENT_TEST_SOURCES
        main.cpp
        )

add_executable(duk.event.test ${DUK_EVENT_TEST_SOURCES})

target_include_directories(duk.event.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk.event.test
        PRIVATE
        duk::event
        )
