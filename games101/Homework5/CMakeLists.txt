cmake_minimum_required(VERSION 3.10)
project(RayTracing)

set(CMAKE_CXX_STANDARD 17)

add_executable(RayTracing main.cpp Object.h Vector.h Sphere.h global.h Triangle.h Scene.h Light.h Renderer.cpp)
#target_compile_options(RayTracing PUBLIC -Wall -Wextra -pedantic -Wshadow -Wreturn-type -fsanitize=undefined)
#target_compile_features(RayTracing PUBLIC cxx_std_17)
#target_link_libraries(RayTracing PUBLIC -fsanitize=undefined)
