project(duk_hash)

set(DUK_HASH_HEADERS
        include/duk_hash/hash.h
        include/duk_hash/hash_combine.h
        include/duk_hash/common_hashes.h
        )

set(DUK_HASH_SOURCES
        src/duk_hash/data_source.cpp
        )

add_library(duk.hash ${DUK_HASH_HEADERS} ${DUK_HASH_SOURCES})
add_library(duk::hash ALIAS duk.hash)

target_include_directories(duk.hash
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )

target_link_libraries(duk.hash
        PUBLIC
        duk::macros
        duk::math
        glm::glm
        )

set_target_properties(duk.hash PROPERTIES
        EXPORT_NAME hash
        )

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.hash
        EXPORT duk-targets
)