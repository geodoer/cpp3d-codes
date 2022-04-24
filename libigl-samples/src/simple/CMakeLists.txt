cmake_minimum_required(VERSION 2.8.12)
project(simple)

#根据一个cpp文件，创建一个简单的示例
macro(add_simple_cpp file)
  # 检查依赖项
  foreach(module IN ITEMS ${ARGN})
      if(NOT TARGET ${module})
          return()
      endif()
  endforeach()

  message(STATUS "Creating libigl simple: ${name}")
  get_filename_component(name ${file} NAME_WE)  #工程名
  add_executable(${name} ${file} ${Project_Headers_Files})
  target_link_libraries(${name} PRIVATE
      igl::core
      ${ARGN}
  )

  # 子工程的文件夹
  set_property(TARGET ${name} PROPERTY FOLDER ${PROJECT_NAME})
  # 代码分组
  source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${Project_Headers_Files})
endmacro()

add_simple_cpp("HiLigIgl.cpp"       igl::glfw)

add_simple_cpp("ui/106ViewerMenu.cpp"  igl::imgui)
add_simple_cpp("ui/107MultipleMeshes.cpp"  igl::imgui)

add_simple_cpp("cgal/Subdivision_method.cpp" igl_copyleft::cgal)