if (CMAKE_GENERATOR MATCHES "Ninja")
    set(VCPKG_TARGET_TRIPLET x64-mingw-static)
endif()
include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)