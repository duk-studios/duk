
set(DUK_SERIAL_HEADERS
        include/duk_serial/rapidjson_import.h
        include/duk_serial/json.h
)

set(DUK_SERIAL_SOURCES
        src/duk_serial/json.cpp
)

add_library(duk.serial ${DUK_SERIAL_HEADERS} ${DUK_SERIAL_SOURCES})
add_library(duk::serial ALIAS duk.serial)

target_include_directories(duk.serial
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

     )

target_link_libraries(duk.serial
        PUBLIC
        duk::type
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