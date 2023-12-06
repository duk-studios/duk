project(duk_import)

set(DUK_IMPORT_HEADERS
        include/duk_import/image/image_importer.h
        include/duk_import/image/image_importer_stb.h
        include/duk_import/importer.h
        )
set(DUK_IMPORT_SOURCES
        src/duk_import/image/image_importer.cpp
        src/duk_import/image/image_importer_stb.cpp
        src/duk_import/importer.cpp
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
        duk_rhi
        PRIVATE
        stb_image
        RapidJSON
)

set_target_properties(duk_import PROPERTIES CXX_STANDARD 20)
