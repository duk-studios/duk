project(duk_math)

set(DUK_MATH_HEADERS
        include/duk_math/math.h
        include/duk_math/aabb.h
        )

set(DUK_MATH_SOURCES
        src/duk_math/math.cpp
        src/duk_math/aabb.cpp
        )

add_library(duk.math STATIC ${DUK_MATH_HEADERS} ${DUK_MATH_SOURCES})
add_library(duk::math ALIAS duk.math)

target_include_directories(duk.math
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
     )

target_link_libraries(duk.math
        PUBLIC
        duk::macros
        glm::glm
        )


set_target_properties(duk.math PROPERTIES
        EXPORT_NAME math
        )

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.math
        EXPORT duk-targets
)