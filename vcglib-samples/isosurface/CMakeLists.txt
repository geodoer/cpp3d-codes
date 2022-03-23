cmake_minimum_required(VERSION 2.8.12)
project(vcglibisosurface)
set(TARGET_NAME vcglibisosurface)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE Headers_FILES "src/*.h" "src/*.hpp")
set(all_files ${SRC_FILES} ${Headers_FILES})

add_executable(${TARGET_NAME} ${all_files})
target_link_libraries(${TARGET_NAME} ${CONAN_LIBS})
