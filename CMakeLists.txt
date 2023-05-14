cmake_minimum_required(VERSION 3.24)
project(duk)

add_subdirectory(external)

add_subdirectory(duk_macros)
add_subdirectory(duk_hash)
add_subdirectory(duk_events)
add_subdirectory(duk_platform)
add_subdirectory(duk_renderer)
add_subdirectory(duk_sample)