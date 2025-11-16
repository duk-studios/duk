
set(DUK_API_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(DUK_API_INC_DIR ${DUK_API_ROOT}/include/duk_api)
set(DUK_API_SRC_DIR ${DUK_API_ROOT}/src/duk_api)

set(DUK_API_HEADERS
        include/duk_api/api.h
        include/duk_api/register_types.h
)
set(DUK_API_SOURCES
        src/duk_api/register_types.cpp
)

add_library(duk.api ${DUK_API_HEADERS} ${DUK_API_SOURCES})
add_library(duk::api ALIAS duk.api)

target_include_directories(duk.api
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk.api
        PUBLIC
        duk::engine
        duk::animation
)

set_target_properties(duk.api PROPERTIES EXPORT_NAME api)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.api
        EXPORT duk-targets
)
