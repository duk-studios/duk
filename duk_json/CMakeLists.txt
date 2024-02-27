project(duk_json)

set(DUK_JSON_HEADERS
        include/duk_json/types.h
        include/duk_json/string.h
        include/duk_json/rapidjson.h
        )

set(DUK_JSON_SOURCES
        src/duk_json/string.cpp
        )

add_library(duk_json STATIC ${DUK_JSON_HEADERS} ${DUK_JSON_SOURCES})

target_include_directories(duk_json
        PUBLIC
        include
     )

target_link_libraries(duk_json
        PUBLIC
        RapidJSON
        glm::glm
        duk_macros
)

set_target_properties(duk_json PROPERTIES CXX_STANDARD 20)
