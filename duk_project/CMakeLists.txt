project(duk_project)

set(DUK_PROJECT_HEADERS
        include/duk_project/resource.h
        include/duk_project/project.h
        )
set(DUK_PROJECT_SOURCES
        src/duk_project/resource.cpp
        src/duk_project/project.cpp
        )

add_library(duk_project STATIC ${DUK_PROJECT_HEADERS} ${DUK_PROJECT_SOURCES})

target_include_directories(duk_project
        PUBLIC
        include

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_project
        PUBLIC
        duk_resource
        duk_import
        duk_tools
        duk_engine
        )

set_target_properties(duk_project PROPERTIES CXX_STANDARD 20)