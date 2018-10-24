## Dependencies: 
VTK, OpenCV, Graphics card, ITK, ITKVTKGlue On.

### Files Needed:
Needs the mhd, raw data, distortion correction, background.png, and tif files to run
## Issues: 
1. Tiff reader not yet working due to weird header for vtk
2. speed of framebuffer better but I think we can still make it faster
3. Not fully utilizing the graphics card for the computations which could be why some of the rotating is slower
## Common crash issues
1. need to click on each of the windows before pressing s (the windows dont update after camera is updated)
2. not pressing s twice before generating a metric will crash since the xray data is not set (no longer crashes but will produce NaN values)
3. files must be in the executable directory for the program to run.
4. opencv must be setup to run with opengl
## To-do: 
1. Get tiff reader working for the weird header
2. Do math to lock camera distance from the render for size
  . now that it is locked we need to figure out the math to get all the sizing correct
3. Get gradient kernel
4. get some sort of metric
5. Get second camera working to layer it over the corresponding camera view
6. write some sort of a header file format for all of this to automate it
7. Improve the speed of the spinning
8. Improve the blending of the image to not blend the black from the ct bone
9. make everthing 64 bit
