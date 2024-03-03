project(duk_serial)

set(DUK_SERIAL_HEADERS
        include/duk_serial/serializer.h
        include/duk_serial/json_serializer.h
        )
set(DUK_SERIAL_SOURCES
        src/duk_serial/serializer.cpp
        src/duk_serial/json_serializer.cpp
        )

add_library(duk_serial STATIC ${DUK_SERIAL_HEADERS} ${DUK_SERIAL_SOURCES})

target_include_directories(duk_serial
        PUBLIC
        include

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
     )

target_link_libraries(duk_serial
        PUBLIC
        duk_log
        glm::glm
        RapidJSON
        )

set_target_properties(duk_serial PROPERTIES CXX_STANDARD 20)

add_subdirectory(test)