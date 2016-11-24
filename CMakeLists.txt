cmake_minimum_required(VERSION 3.5)
project(9517assignment1)


FIND_PACKAGE(OpenCV REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(9517assignment1 ${SOURCE_FILES})

target_link_libraries(9517assignment1 ${OpenCV_LIBS} )