cmake_minimum_required(VERSION 2.8)
 
PROJECT(MetaImageWriter)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
 
add_executable(MetaImageWriter MACOSX_BUNDLE MetaImageWriter.cxx)
add_executable(writemhd MACOSX_BUNDLE writemhd.cpp)

if(VTK_LIBRARIES)
  target_link_libraries(MetaImageWriter ${VTK_LIBRARIES})
else()
  target_link_libraries(MetaImageWriter vtkHybrid vtkWidgets)
endif()