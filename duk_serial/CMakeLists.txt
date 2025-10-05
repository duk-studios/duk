
set(DUK_SERIAL_HEADERS
        include/duk_serial/rapidjson_import.h
        include/duk_serial/serial.h
        include/duk_serial/parser.h
        include/duk_serial/glm_parsers.h
        include/duk_serial/container_parsers.h
        )

add_library(duk.serial INTERFACE ${DUK_SERIAL_HEADERS})
add_library(duk::serial ALIAS duk.serial)

target_include_directories(duk.serial
        INTERFACE
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

     )

target_link_libraries(duk.serial
        INTERFACE
        duk::log
        duk::math
        rapidjson
        )

set_target_properties(duk.serial PROPERTIES EXPORT_NAME serial)

if (DUK_BUILD_TESTS)
    add_subdirectory(test)
endif ()

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.serial
        EXPORT duk-targets
)