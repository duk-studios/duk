project(duk_system)

set(DUK_SYSTEM_HEADERS
        include/duk_system/system.h
        include/duk_system/system_solver.h
)

set(DUK_SYSTEM_SOURCES
        src/duk_system/system.cpp
)

add_library(duk.system ${DUK_SYSTEM_SOURCES} ${DUK_SYSTEM_HEADERS})
add_library(duk::system ALIAS duk.system)

target_include_directories(duk.system
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk.system
        PUBLIC
        duk::tools
        duk::log
        duk::resource
        duk::objects
)

set_target_properties(duk.system PROPERTIES EXPORT_NAME system)

add_subdirectory(test)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.system
        EXPORT duk-targets
)
