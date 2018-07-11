cmake_minimum_required(VERSION 2.8)
 
PROJECT(visualizeDicom)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
 
add_executable(visualizeDicom MACOSX_BUNDLE visualizeDicom.cxx)
 
if(VTK_LIBRARIES)
  target_link_libraries(visualizeDicom ${VTK_LIBRARIES})
else()
  target_link_libraries(visualizeDicom vtkHybrid vtkWidgets)
endif()