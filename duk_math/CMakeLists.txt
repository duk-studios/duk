project(duk_math)

set(DUK_MATH_HEADERS
        include/duk_math/math.h
        include/duk_math/aabb.h
        )

set(DUK_MATH_SOURCES
        src/duk_math/math.cpp
        src/duk_math/aabb.cpp
        )

add_library(duk_math STATIC ${DUK_MATH_HEADERS} ${DUK_MATH_SOURCES})
add_library(duk::duk_math ALIAS duk_math)

target_include_directories(duk_math
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
     )

target_link_libraries(duk_math
        PUBLIC
        duk_macros
        glm::glm
        )


set_target_properties(duk_math PROPERTIES
        CXX_STANDARD 20
        )

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk_math
        EXPORT duk-targets
)