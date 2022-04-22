cmake_minimum_required(VERSION 3.11)
project(libigl-samples)

#将./cmake目录加入搜索路径
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

#
# Libigl
#
include(libigl)

#
# 项目配置
# 

#工程公共头文件
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)  #工程头文件目录
include_directories(${PROJECT_INCLUDE_DIR})           #添加include目录（全局）
file(GLOB_RECURSE Project_Headers_Files ${PROJECT_INCLUDE_DIR}/*.h ${PROJECT_INCLUDE_DIR}/*.hpp)  #获得工程目录下的头文件

#
# 添加子模块
# 

file(GLOB_RECURSE cmakelists_files ${CMAKE_SOURCE_DIR}/src/*/CMakeLists.txt)  #递归列出子CMakeLists.txt文件
foreach(file ${cmakelists_files})
  get_filename_component(subdir ${file} DIRECTORY)  #获得子CMakeLists.txt文件所在目录
  add_subdirectory(${subdir}) #添加子模块
endforeach()

#
# 其他配置
#

# 开启工程分组
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
#关闭编译器优化
set(CMAKE_CXX_FLAGS "-fno-elide-constructors ${CMAKE_CXX_FLAGS}")