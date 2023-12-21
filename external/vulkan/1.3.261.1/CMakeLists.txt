add_library(duk_Vulkan STATIC IMPORTED GLOBAL)

set_target_properties(duk_Vulkan PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/Lib/vulkan-1.lib")

target_include_directories(duk_Vulkan INTERFACE Include)

add_library(Vulkan::Vulkan ALIAS duk_Vulkan)
