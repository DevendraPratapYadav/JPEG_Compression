cmake_minimum_required(VERSION 2.8.12)

project(jpeg)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(jpeg jpeg.cpp)
target_link_libraries(jpeg ${OpenCV_LIBS})
