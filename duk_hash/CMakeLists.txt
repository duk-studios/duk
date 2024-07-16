project(duk_hash)

set(DUK_HASH_HEADERS
        include/duk_hash/hash.h
        include/duk_hash/hash_combine.h
        include/duk_hash/common_hashes.h
        )

set(DUK_HASH_SOURCES
        src/duk_hash/data_source.cpp
        )

add_library(duk_hash STATIC ${DUK_HASH_HEADERS} ${DUK_HASH_SOURCES})
add_library(duk::duk_hash ALIAS duk_hash)

target_include_directories(duk_hash
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )

target_link_libraries(duk_hash
        PUBLIC
        duk_macros
        glm::glm
        duk_math
        )

set_target_properties(duk_hash PROPERTIES
        CXX_STANDARD 20
        )

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk_hash
        EXPORT duk-targets
)