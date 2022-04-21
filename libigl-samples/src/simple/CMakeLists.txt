cmake_minimum_required(VERSION 2.8.12)
project(simple)

file(GLOB files "*.cpp")
foreach(file ${files})
  #工程名
  get_filename_component(name ${file} NAME_WE)

  #创建工程 
	add_executable(${name} ${file} ${Project_Headers_Files})
	target_link_libraries(${name} PRIVATE igl::glfw)

  # 子工程的文件夹
  set_property(TARGET ${name} PROPERTY FOLDER ${PROJECT_NAME})
  # 代码分组
  source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${Project_Headers_Files})
endforeach()