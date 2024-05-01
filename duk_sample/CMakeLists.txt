project(duk_sample)

if (NOT MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif ()

set(DUK_SAMPLE_HEADERS
        )

set(DUK_SAMPLE_SOURCES
        src/duk_sample/api.cpp
        src/duk_sample/camera_system.h
        src/duk_sample/camera_system.cpp
        src/duk_sample/sprite_animator_system.h
        src/duk_sample/sprite_animator_system.cpp
        src/duk_sample/stats_system.h
        src/duk_sample/stats_system.cpp
)

add_library(duk_sample STATIC ${DUK_SAMPLE_HEADERS} ${DUK_SAMPLE_SOURCES})

target_include_directories(duk_sample
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_sample
        PRIVATE
        duk_api
        )

set_target_properties(duk_sample PROPERTIES CXX_STANDARD 20)

target_link_libraries(duk_runtime PRIVATE duk_sample)
