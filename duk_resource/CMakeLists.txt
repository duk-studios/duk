project(duk_resource)

set(DUK_RESOURCE_HEADERS
        include/duk_resource/file.h
        include/duk_resource/handler.h
        include/duk_resource/pool.h
        include/duk_resource/handle.h
        include/duk_resource/resources.h
        include/duk_resource/solver/reference_solver.h
        include/duk_resource/solver/dependency_solver.h
        include/duk_resource/solver/solver.h
        )
set(DUK_RESOURCE_SOURCES
        src/duk_resource/file.cpp
        src/duk_resource/handler.cpp
        src/duk_resource/pool.cpp
        src/duk_resource/handle.cpp
        src/duk_resource/resources.cpp
        src/duk_resource/solver/reference_solver.cpp
        src/duk_resource/solver/dependency_solver.cpp
        )

add_library(duk.resource ${DUK_RESOURCE_HEADERS} ${DUK_RESOURCE_SOURCES})
add_library(duk::resource ALIAS duk.resource)

target_link_libraries(duk.resource
        PUBLIC
        duk::macros
        duk::serial
        )

target_include_directories(duk.resource
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

set_target_properties(duk.resource PROPERTIES EXPORT_NAME resource)

add_subdirectory(test)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.resource
        EXPORT duk-targets
)