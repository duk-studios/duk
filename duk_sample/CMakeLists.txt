project(duk_sample)

if (NOT MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif ()

set(DUK_SAMPLE_HEADERS
        )

set(DUK_SAMPLE_SOURCES
        src/duk_sample/main.cpp
        src/duk_sample/application.h
        src/duk_sample/application.cpp
        src/duk_sample/camera_system.h
        src/duk_sample/camera_system.cpp
)

add_executable(duk_sample ${DUK_SAMPLE_HEADERS} ${DUK_SAMPLE_SOURCES})

target_include_directories(duk_sample
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_sample
        PRIVATE
        duk_engine
        )

set_target_properties(duk_sample PROPERTIES CXX_STANDARD 20)

get_cmake_property(IS_MULTI_CONFIG GENERATOR_IS_MULTI_CONFIG)

set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
if (IS_MULTI_CONFIG)
    set(OUTPUT_DIR "${OUTPUT_DIR}/$<CONFIG>")
endif ()

set(DUK_SAMPLE_RESOURCES
        resources/images/gold_basecolor.png
        resources/images/gold_basecolor.res
        resources/images/gold_specular.png
        resources/images/gold_specular.res
        resources/images/safeson.png
        resources/images/safeson.res
        resources/images/uv.jpg
        resources/images/uv.res
        resources/materials/phong/blue.mat
        resources/materials/phong/blue.res
        resources/materials/phong/cyan.mat
        resources/materials/phong/cyan.res
        resources/materials/phong/green.mat
        resources/materials/phong/green.res
        resources/materials/phong/magenta.mat
        resources/materials/phong/magenta.res
        resources/materials/phong/red.mat
        resources/materials/phong/red.res
        resources/materials/phong/yellow.mat
        resources/materials/phong/yellow.res
        resources/materials/color.mat
        resources/materials/color.res
        resources/materials/light.mat
        resources/materials/light.res
        resources/materials/phong.mat
        resources/materials/phong.res
        resources/materials/safeson.mat
        resources/materials/safeson.res
        resources/scenes/directions.res
        resources/scenes/directions.scn
        resources/scenes/empty.res
        resources/scenes/empty.scn
        resources/scenes/scene.res
        resources/scenes/scene.scn
        resources/sample.duk
)
add_custom_command(
        OUTPUT _resource_copy_timestamp.txt
        DEPENDS ${DUK_SAMPLE_RESOURCES}
        COMMENT "Copying resources to ${OUTPUT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "${OUTPUT_DIR}"
)

# Need to create a custom target for custom command to work
add_custom_target(duk_sample_copy_resources ALL
        # Use absolute path for the DEPENDS file.
        # Relative paths are interpreted relative to the source directory.
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/_resource_copy_timestamp.txt
)

add_dependencies(duk_sample duk_sample_copy_resources)