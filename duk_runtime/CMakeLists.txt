if (NOT MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif ()

set(DUK_RUNTIME_SOURCES
        src/duk_runtime/application.h
        src/duk_runtime/main.cpp
        src/duk_runtime/application.cpp
)

add_library(duk.runtime OBJECT ${DUK_RUNTIME_SOURCES})
add_library(duk::runtime ALIAS duk.runtime)

target_include_directories(duk.runtime
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk.runtime
        PUBLIC
        duk::api
)

set_target_properties(duk.runtime PROPERTIES EXPORT_NAME runtime)

install(
        TARGETS duk.runtime
        EXPORT duk-targets
        OBJECTS DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

