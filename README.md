## Dependencies: 
VTK, OpenCV, Graphics card

## Issues: 
1. Tiff reader not yet working due to weird header for vtk
2. speed of framebuffer better but I think we can still make it faster

## To-do: 
1. Get tiff reader working for the weird header
2. Do math to lock camera distance from the render for size
3. Get gradient kernel
4. get some sort of metric
5. Get second camera working to layer it over the corresponding camera view
6. write some sort of a header file format for all of this to automate it
7. Improve the speed of the spinning
8. Improve the blending of the image to not blend the black from the ct bone
