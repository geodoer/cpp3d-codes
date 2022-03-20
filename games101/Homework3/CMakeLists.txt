cmake_minimum_required(VERSION 3.10)
project(Rasterizer)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_CXX_STANDARD 17)

add_executable(Rasterizer main.cpp rasterizer.hpp rasterizer.cpp global.hpp Triangle.hpp Triangle.cpp Texture.hpp Texture.cpp Shader.hpp OBJ_Loader.h)
target_link_libraries(Rasterizer ${CONAN_LIBS})
#target_compile_options(Rasterizer PUBLIC -Wall -Wextra -pedantic)