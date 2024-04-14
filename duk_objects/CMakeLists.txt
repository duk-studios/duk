project(duk_objects)

set(DUK_OBJECTS_HEADERS
        include/duk_objects/objects.h
        include/duk_objects/objects_pool.h
        include/duk_objects/objects_handler.h
        include/duk_objects/register_types.h
)

set(DUK_OBJECTS_SOURCES
        src/duk_objects/objects.cpp
        src/duk_objects/objects_pool.cpp
        src/duk_objects/objects_handler.cpp
        src/duk_objects/register_types.cpp
)

add_library(duk_objects STATIC ${DUK_OBJECTS_SOURCES} ${DUK_OBJECTS_HEADERS})
add_library(duk::duk_objects ALIAS duk_objects)

target_include_directories(duk_objects
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_objects
        PUBLIC
        duk_macros
        duk_hash
        duk_event
        duk_tools
        duk_log
        duk_resource
        )

set_target_properties(duk_objects PROPERTIES CXX_STANDARD 20)

add_subdirectory(test)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk_objects
        EXPORT duk-targets
)
