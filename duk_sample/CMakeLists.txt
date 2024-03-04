project(duk_sample)

if (NOT MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif ()

set(DUK_SAMPLE_HEADERS
        )

set(DUK_SAMPLE_SOURCES
        src/duk_sample/main.cpp
        src/duk_sample/application.h
        src/duk_sample/application.cpp
        src/duk_sample/camera_system.h
        src/duk_sample/camera_system.cpp
)

add_executable(duk_sample ${DUK_SAMPLE_HEADERS} ${DUK_SAMPLE_SOURCES})

target_include_directories(duk_sample
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_sample
        PRIVATE
        duk_engine
        )

set_target_properties(duk_sample PROPERTIES CXX_STANDARD 20)
