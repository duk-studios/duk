
set(DUK_LOG_TEST_SOURCES
        main.cpp
        )

if (NOT MSVC)
        set(CMAKE_EXE_LINKER_FLAGS "-static")
endif ()

add_executable(duk.log.test ${DUK_LOG_TEST_SOURCES})

target_include_directories(duk.log.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk.log.test
        PRIVATE
        duk::log
        )
