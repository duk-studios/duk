project(duk_system)

set(DUK_SYSTEM_HEADERS
        include/duk_system/system.h
        include/duk_system/system_solver.h
)

set(DUK_SYSTEM_SOURCES
        src/duk_system/system.cpp
)

add_library(duk_system STATIC ${DUK_SYSTEM_SOURCES} ${DUK_SYSTEM_HEADERS})
add_library(duk::duk_system ALIAS duk_system)

target_include_directories(duk_system
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk_system
        PUBLIC
        duk_tools
        duk_log
        duk_resource
        duk_objects
)

set_target_properties(duk_system PROPERTIES CXX_STANDARD 20)

add_subdirectory(test)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk_system
        EXPORT duk-targets
)
