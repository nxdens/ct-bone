# CMakeLists.txt file for building "bonezone" project
# Must first download VTK, ITK, and OpenCV, build a project for each using
#   CMake, and then compile each. 
# May also need to download ItkVtkGlue, though this may be included by checking the 
#   "Module_ITKVtkGlue" option in Cmake when creating the project for ITK
# Must run CMake on VTK first, since you will need to specify the path to VTK
#   when you run CMake on ITK or ItkVtkGlue

cmake_minimum_required(VERSION 3.9.5)

project(bonezone)

find_package(ITK REQUIRED)

include(${ITK_USE_FILE})
if(ITKVtkGlue_LOADED)
	find_package(VTK REQUIRED)
  	include(${VTK_USE_FILE})
 else()
 	find_package(ItkVtkGlue REQUIRED)
 	include(${ItkVtkGlue_USE_FILE})
  	set(Glue ItkVtkGlue)

endif()
find_package( OpenCV REQUIRED )
add_executable(bonezone MACOSX_BUNDLE bonezone.cpp dsxBone.cpp dsxCube.cpp dsxGrabBufferActor.cpp dsxGrabBufferCamera.cpp dsxGUI.cpp dsxImageSequence.cpp dsxMetric.cpp dsxOptimizer.cpp dsxPose.cpp dsxProject.cpp dsxReRenderer.cpp dsxTiffMovie.cpp dsxVolumeSpinner.cpp dsxXrayImage.cpp)
target_link_libraries(bonezone  ${Glue}  ${VTK_LIBRARIES} ${ITK_LIBRARIES} ${OpenCV_LIBS})
