include_guard()

if (CMAKE_GENERATOR MATCHES "Ninja")
    set(VCPKG_TARGET_TRIPLET x64-mingw-static)
endif()
set(VCPKG_MANIFEST_DIR ${DUK_ROOT})
include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
