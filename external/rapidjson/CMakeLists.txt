# rapidjson does not provide a proper target for linkage,
# so we need to create an interface library

add_library(RapidJSON INTERFACE)

target_include_directories(RapidJSON INTERFACE rapidjson/include)