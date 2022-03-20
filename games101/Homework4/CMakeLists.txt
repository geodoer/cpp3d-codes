cmake_minimum_required(VERSION 3.10)
project(BezierCurve)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_CXX_STANDARD 14)

add_executable(BezierCurve main.cpp)
target_link_libraries(BezierCurve ${CONAN_LIBS})
