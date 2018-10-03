## Dependencies: 
VTK, OpenCV, Graphics card, ITK, ITKVTKGlue On.
##Files Needed:
Needs the mhd, raw data, distortion correction, background.png, and tif files to run
## Issues: 
1. Tiff reader not yet working due to weird header for vtk
2. speed of framebuffer better but I think we can still make it faster
3. Not fully utilizing the graphics card for the computations which could be why some of the rotating is slower

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
