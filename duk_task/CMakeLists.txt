project(duk_task)

set(DUK_TASK_HEADERS
        include/duk_task/task_queue.h
        )
set(DUK_TASK_SOURCES
        src/duk_task/task_queue.cpp
        )

add_library(duk.task STATIC ${DUK_TASK_HEADERS} ${DUK_TASK_SOURCES})
add_library(duk::task ALIAS duk.task)

target_link_libraries(duk.task PUBLIC duk::macros)

target_include_directories(duk.task
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

set_target_properties(duk.task PROPERTIES EXPORT_NAME task)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.task
        EXPORT duk-targets
)