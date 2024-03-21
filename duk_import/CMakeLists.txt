project(duk_import)

set(DUK_IMPORT_HEADERS
        include/duk_import/importer.h
        include/duk_import/image/image_importer.h
        include/duk_import/image/image_loader_stb.h
        include/duk_import/material/material_importer.h
        include/duk_import/material/material_loader_json.h
        include/duk_import/resource_set_importer.h
        include/duk_import/scene/scene_importer.h
        include/duk_import/scene/scene_loader_json.h
        include/duk_import/resource_importer.h
        )
set(DUK_IMPORT_SOURCES
        src/duk_import/importer.cpp
        src/duk_import/image/image_importer.cpp
        src/duk_import/image/image_loader_stb.cpp
        src/duk_import/material/material_importer.cpp
        src/duk_import/material/material_loader_json.cpp
        src/duk_import/resource_set_importer.cpp
        src/duk_import/scene/scene_importer.cpp
        src/duk_import/scene/scene_loader_json.cpp
        src/duk_import/resource_importer.cpp
        )

add_library(duk_import STATIC ${DUK_IMPORT_HEADERS} ${DUK_IMPORT_SOURCES})

target_include_directories(duk_import
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        ${Stb_INCLUDE_DIR}
     )

target_link_libraries(duk_import
        PUBLIC
        duk_macros
        duk_tools
        duk_renderer
)

set_target_properties(duk_import PROPERTIES CXX_STANDARD 20)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk_import
        EXPORT duk-targets
)
