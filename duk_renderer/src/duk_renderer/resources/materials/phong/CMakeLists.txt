generate_material_files(
        GLSL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/phong.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/glsl/phong.frag
        NAME "phong"
        INC_DIR ${DUK_MATERIAL_INC_DIR}/phong
        SRC_DIR ${DUK_MATERIAL_SRC_DIR}/phong
)