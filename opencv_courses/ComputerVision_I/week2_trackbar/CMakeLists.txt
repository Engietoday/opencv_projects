cmake_minimum_required(VERSION 2.8.3)
project(trackbar)

add_compile_options(-std=c++11)

find_package(OpenCV 4 REQUIRED)

add_executable(main src/trackbar.cpp)
target_link_libraries(main ${OpenCV_LIBS})
