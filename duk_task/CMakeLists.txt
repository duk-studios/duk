project(duk_task)

set(DUK_TASK_HEADERS
        include/duk_task/task_queue.h
        )
set(DUK_TASK_SOURCES
        src/duk_task/task_queue.cpp
        )

add_library(duk_task STATIC ${DUK_TASK_HEADERS} ${DUK_TASK_SOURCES})

target_include_directories(duk_task
     PUBLIC
     include

     PRIVATE
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
     )

set_target_properties(duk_task PROPERTIES CXX_STANDARD 20)

