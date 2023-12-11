project(duk_import)

set(DUK_IMPORT_HEADERS
        include/duk_import/importer.h
        include/duk_import/json/types.h
        include/duk_import/scene/component_json_parser.h
        include/duk_import/image/image_importer.h
        include/duk_import/image/image_importer_stb.h
        include/duk_import/material/material_importer.h
        include/duk_import/material/material_importer_json.h
        include/duk_import/resource/resource_importer.h
        include/duk_import/resource/resource_importer_json.h
        include/duk_import/scene/scene_importer.h
        include/duk_import/scene/scene_importer_json.h
        )
set(DUK_IMPORT_SOURCES
        src/duk_import/importer.cpp
        src/duk_import/json/types.cpp
        src/duk_import/scene/component_json_parser.cpp
        src/duk_import/image/image_importer.cpp
        src/duk_import/image/image_importer_stb.cpp
        src/duk_import/material/material_importer.cpp
        src/duk_import/material/material_importer_json.cpp
        src/duk_import/resource/resource_importer.cpp
        src/duk_import/resource/resource_importer_json.cpp
        src/duk_import/scene/scene_importer.cpp
        src/duk_import/scene/scene_importer_json.cpp
        )

add_library(duk_import STATIC ${DUK_IMPORT_HEADERS} ${DUK_IMPORT_SOURCES})

target_include_directories(duk_import
     PUBLIC
     include

     PRIVATE
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
     )

target_link_libraries(duk_import
        PUBLIC
        duk_macros
        duk_tools
        duk_renderer
        RapidJSON
        PRIVATE
        stb_image
)

set_target_properties(duk_import PROPERTIES CXX_STANDARD 20)
