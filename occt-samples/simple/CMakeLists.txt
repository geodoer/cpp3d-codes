cmake_minimum_required(VERSION 2.8.12)
project(occtsamples-simple)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB files "src/*.cpp")
foreach(file ${files})
  get_filename_component(name ${file} NAME_WE)
  add_executable(${name} ${file})
  target_link_libraries(${name} ${CONAN_LIBS})
endforeach()