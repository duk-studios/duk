add_subdirectory(cxxopts)

if (NOT TARGET cxxopts::cxxopts)
    message(FATAL_ERROR "failed to create target cxxopts")
endif ()