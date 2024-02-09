project(duk_resource)

set(DUK_RESOURCE_HEADERS
        include/duk_resource/resource.h
        include/duk_resource/pool.h
        include/duk_resource/resource_solver.h
        )
set(DUK_RESOURCE_SOURCES
        src/duk_resource/resource.cpp
        src/duk_resource/pool.cpp
        src/duk_resource/resource_solver.cpp
        )

add_library(duk_resource STATIC ${DUK_RESOURCE_HEADERS} ${DUK_RESOURCE_SOURCES})

target_link_libraries(duk_resource
        PUBLIC
        duk_macros
        duk_json
        )

target_include_directories(duk_resource
        PUBLIC
        include

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

set_target_properties(duk_resource PROPERTIES CXX_STANDARD 20)

