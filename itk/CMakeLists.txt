cmake_minimum_required(VERSION 3.9.5)

project(DSX_Project)

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
add_executable(DSX_Project MACOSX_BUNDLE DSX_Project.cpp dsxGUI.cpp dsxImageSequence.cpp dsxMetric.cpp dsxOptimizer.cpp dsxCube.cpp dsxTiffMovie.cpp dsxGrabBufferActor.cpp dsxGrabBufferCamera.cpp dsxReRenderer.cpp dsxVolumeSpinner.cpp dsxXrayImage.cpp)
target_link_libraries(DSX_Project
  ${Glue}  ${VTK_LIBRARIES} ${ITK_LIBRARIES} ${OpenCV_LIBS})
