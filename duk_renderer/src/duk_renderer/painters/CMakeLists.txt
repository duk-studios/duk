
# Try to find the glslc executable from the Vulkan SDK
find_program(GLSLC_EXECUTABLE glslc HINTS ENV VULKAN_SDK PATH_SUFFIXES "Bin")

if (NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found. Make sure the Vulkan SDK is correctly installed and its bin directory is in your PATH.")
endif()

# Define a list of source and destination paths
set(SHADER_SPV_GENERATE_COMMANDS
        COMMAND ${GLSLC_EXECUTABLE} glsl/color/color.vert -o spv/color/color.vert.spv
        COMMAND ${GLSLC_EXECUTABLE} glsl/color/color.frag -o spv/color/color.frag.spv
        # Add more arguments as needed
        )

add_custom_target(generate_spv_files
        ${SHADER_SPV_GENERATE_COMMANDS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating SPV files"
        )

add_dependencies(duk_renderer generate_spv_files)
