project(duk_resource)

set(DUK_RESOURCE_HEADERS
        include/duk_resource/file.h
        include/duk_resource/handler.h
        include/duk_resource/pool.h
        include/duk_resource/resource.h
        include/duk_resource/set.h
        include/duk_resource/solver/reference_solver.h
        include/duk_resource/solver/dependency_solver.h
        include/duk_resource/solver/solver.h
        )
set(DUK_RESOURCE_SOURCES
        src/duk_resource/file.cpp
        src/duk_resource/handler.cpp
        src/duk_resource/pool.cpp
        src/duk_resource/resource.cpp
        src/duk_resource/set.cpp
        src/duk_resource/solver/reference_solver.cpp
        src/duk_resource/solver/dependency_solver.cpp
        )

add_library(duk_resource STATIC ${DUK_RESOURCE_HEADERS} ${DUK_RESOURCE_SOURCES})
add_library(duk::duk_resource ALIAS duk_resource)

target_link_libraries(duk_resource
        PUBLIC
        duk_macros
        duk_serial
        )

target_include_directories(duk_resource
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

set_target_properties(duk_resource PROPERTIES CXX_STANDARD 20)

add_subdirectory(test)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk_resource
        EXPORT duk-targets
)