option(SPIRV_REFLECT_STATIC_LIB "Enables static libs" ON)
option(SPIRV_REFLECT_EXECUTABLE "Enables executable" OFF)
option(SPIRV_REFLECT_EXAMPLES "Enables examples" OFF)

add_subdirectory(SPIRV-Reflect)

if (NOT TARGET spirv-reflect-static)
    message(FATAL_ERROR "failed to create target spirv-reflect-static")
endif ()