project(duk_serial)

set(DUK_SERIAL_HEADERS
        include/duk_serial/json/rapidjson_import.h
        include/duk_serial/json/serializer.h
        include/duk_serial/json/types.h
        )

add_library(duk_serial INTERFACE ${DUK_SERIAL_HEADERS})
add_library(duk::duk_serial ALIAS duk_serial)

target_include_directories(duk_serial
        INTERFACE
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

     )

target_link_libraries(duk_serial
        INTERFACE
        duk_log
        glm::glm
        rapidjson
        )

set_target_properties(duk_serial PROPERTIES CXX_STANDARD 20)

add_subdirectory(test)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk_serial
        EXPORT duk-targets
)