set(DUK_TYPE_HEADERS
        include/duk_type/type.h
        )

set(DUK_TYPE_SOURCES
        src/duk_type/type.cpp
)

add_library(duk.type STATIC ${DUK_TYPE_HEADERS} ${DUK_TYPE_SOURCES})
add_library(duk::type ALIAS duk.type)

target_include_directories(duk.type
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

     )

target_link_libraries(duk.type
        PUBLIC
        duk::log
        )

set_target_properties(duk.type PROPERTIES EXPORT_NAME type)

if (DUK_BUILD_TESTS)
    add_subdirectory(test)
endif ()

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.type
        EXPORT duk-targets
)