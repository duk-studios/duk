project(duk_task)

set(DUK_TASK_HEADERS
        include/duk_task/task_queue.h
        )
set(DUK_TASK_SOURCES
        src/duk_task/task_queue.cpp
        )

add_library(duk_task STATIC ${DUK_TASK_HEADERS} ${DUK_TASK_SOURCES})
add_library(duk::duk_task ALIAS duk_task)

target_link_libraries(duk_task PUBLIC duk_macros)

target_include_directories(duk_task
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

set_target_properties(duk_task PROPERTIES CXX_STANDARD 20)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk_task
        EXPORT duk-targets
)