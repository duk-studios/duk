include_guard()

if (CMAKE_GENERATOR MATCHES "Ninja")
    set(VCPKG_TARGET_TRIPLET x64-mingw-dynamic)
endif()

set(X_VCPKG_APPLOCAL_DEPS_INSTALL TRUE CACHE BOOL "Copy dependencies to install target directory")
set(VCPKG_MANIFEST_DIR ${DUK_ROOT})
include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
