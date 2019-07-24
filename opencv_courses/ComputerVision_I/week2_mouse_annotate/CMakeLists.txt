cmake_minimum_required(VERSION 2.8.3)
project(mouse_annotation)

add_compile_options(-std=c++11)

find_package(OpenCV 4 REQUIRED)

add_executable(main src/mouse_annotation.cpp)
target_link_libraries(main ${OpenCV_LIBS})
