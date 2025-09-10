project(duk_objects)

set(DUK_OBJECTS_HEADERS
        include/duk_objects/events.h
        include/duk_objects/id.h
        include/duk_objects/objects.h
        include/duk_objects/objects_handler.h
        include/duk_objects/register_types.h
)

set(DUK_OBJECTS_SOURCES
        src/duk_objects/id.cpp
        src/duk_objects/objects.cpp
        src/duk_objects/objects_handler.cpp
        src/duk_objects/register_types.cpp
)

add_library(duk.objects ${DUK_OBJECTS_SOURCES} ${DUK_OBJECTS_HEADERS})
add_library(duk::objects ALIAS duk.objects)

target_include_directories(duk.objects
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk.objects
        PUBLIC
        duk::macros
        duk::hash
        duk::event
        duk::tools
        duk::log
        duk::resource
        )

set_target_properties(duk.objects PROPERTIES EXPORT_NAME objects)

add_subdirectory(test)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.objects
        EXPORT duk-targets
)
