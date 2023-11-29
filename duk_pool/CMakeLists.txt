project(duk_pool)

set(DUK_POOL_HEADERS
        include/duk_pool/resource.h
        include/duk_pool/pool.h
        )
set(DUK_POOL_SOURCES
        src/duk_pool/resource.cpp
        src/duk_pool/pool.cpp
        )

add_library(duk_pool STATIC ${DUK_POOL_HEADERS} ${DUK_POOL_SOURCES})

target_link_libraries(duk_pool PUBLIC duk_hash)

target_include_directories(duk_pool
        PUBLIC
        include

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

set_target_properties(duk_pool PROPERTIES CXX_STANDARD 20)

