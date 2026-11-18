cmake_minimum_required(VERSION 3.21)
project(Project2 VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread -O0 -g -w")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    # ${CMAKE_CURRENT_SOURCE_DIR}/other/dirs
)

add_library(
    ${PROJECT_NAME}
    SHARED
    
    # Sources
        src/fake_receiver.c
        src/telemetry_fsm.cpp
        include/telemetry_fsm.h
        include/exceptions.h
)

target_link_libraries(${PROJECT_NAME}
    # Other librs
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(project_2 src/main.cpp
        src/receiver_thread.cpp
        include/receiver_thread.h
        src/parser.cpp
        include/parser.h
        src/session_stats.cpp
        include/session_stats.h)
target_link_libraries(project_2 ${PROJECT_NAME})
