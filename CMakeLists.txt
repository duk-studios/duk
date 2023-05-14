cmake_minimum_required(VERSION 3.23)
project(duk)

set(CMAKE_CXX_STANDARD 20)

set(DUK_HEADERS
        include/duk/renderer/renderer.h
        )

set(DUK_SOURCES
        )

add_library(duk STATIC ${DUK_SOURCES} ${DUK_HEADERS})

target_include_directories(duk PUBLIC include)
