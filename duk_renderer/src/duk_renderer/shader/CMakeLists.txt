include(duk-shader-library)

set(DUK_SHADER_SRC_DIR "${DUK_RENDERER_SRC_DIR}/shader")
set(DUK_SHADER_INC_DIR "${DUK_RENDERER_INC_DIR}/shader")

add_subdirectory(color)
add_subdirectory(fullscreen)
add_subdirectory(phong)
add_subdirectory(text)
add_subdirectory(image)
