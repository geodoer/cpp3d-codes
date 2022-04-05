cmake_minimum_required(VERSION 2.8.12)
project(simple)

find_package(OpenCASCADE
COMPONENTS
  FoundationClasses
  # ModelingAlgorithms
  # ModelingData
  # DataExchange
  # Visualization
REQUIRED
)

find_package(geodoer
REQUIRED)

file(GLOB files "*.cpp")
foreach(file ${files})
  #工程名
  get_filename_component(name ${file} NAME_WE)

  #创建工程 
	add_executable(${name} ${file} ${Project_Headers_Files})
  target_include_directories(${name} PRIVATE ${OpenCASCADE_INCLUDE_DIR})
	target_link_libraries(${name} ${OpenCASCADE_LIBRARIES})

  # 子工程的文件夹
  set_property(TARGET ${name} PROPERTY FOLDER ${PROJECT_NAME})
  # 代码分组
  source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${Project_Headers_Files})
endforeach()