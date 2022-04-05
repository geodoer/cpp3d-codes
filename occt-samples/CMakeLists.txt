cmake_minimum_required(VERSION 2.8.12)
project(occt-samples)

# 让find_package找到conan安装的包
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/build)  #添加到find_package module模式搜索路径
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/build)  #添加到find_package config模式搜索路径
#添加geodoer库的路径
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../geodoer)

#
# 项目配置
# 

#工程公共头文件
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)  #工程头文件目录
include_directories(${PROJECT_INCLUDE_DIR})  #添加include目录（全局）
file(GLOB_RECURSE Project_Headers_Files ${PROJECT_INCLUDE_DIR}/*.h ${PROJECT_INCLUDE_DIR}/*.hpp)

#
# 添加子模块
# 

file(GLOB_RECURSE cmakelists_files ${CMAKE_SOURCE_DIR}/src/*/CMakeLists.txt)
foreach(file ${cmakelists_files})
  get_filename_component(subdir ${file} DIRECTORY)
  add_subdirectory(${subdir})
endforeach()

#
# 其他配置
#

# 开启工程分组
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
#关闭编译器优化
set(CMAKE_CXX_FLAGS “-fno-elide-constructors ${CMAKE_CXX_FLAGS}”)