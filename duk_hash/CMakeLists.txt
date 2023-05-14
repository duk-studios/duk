set(DUK_HASH_HEADERS
        include/duk_hash/hash.h
        include/duk_hash/hash_combine.h
        include/duk_hash/common_hashes.h
        )

add_library(duk_hash INTERFACE ${DUK_HASH_HEADERS})

target_include_directories(duk_hash
        INTERFACE
        include
        )

target_link_libraries(duk_hash
        INTERFACE
        glm::glm
        )

set_target_properties(duk_hash PROPERTIES
        CXX_STANDARD 20
        )
