cmake_minimum_required(VERSION 2.8.3)
project(autofocus)

add_compile_options(-std=c++11)

find_package(OpenCV 4 REQUIRED)

add_executable(main src/main.cpp)
target_link_libraries(main ${OpenCV_LIBS})
