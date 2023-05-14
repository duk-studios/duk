//
// Created by Ricardo on 13/04/2023.
//

#ifndef DUK_VULKAN_IMPORT_H
#define DUK_VULKAN_IMPORT_H

#if DUK_PLATFORM_IS_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#elif DUK_PLATFORM_IS_LINUX
#define VK_USE_PLATFORM_XLIB_KHR
#else
#error Unsupported platform for vulkan
#endif

#include <vulkan/vulkan.h>



#endif //DUK_VULKAN_IMPORT_H
