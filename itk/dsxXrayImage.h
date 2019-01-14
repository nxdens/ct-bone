#ifndef __DSX_XRAYIMAGE__
#define __DSX_XRAYIMAGE__

//all the visualization includes
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeMapper.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkImageData.h>
#include <vtkPiecewiseFunction.h>
#include <vtkUnsignedCharArray.h>
#include <vtkImageBlend.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkImageFlip.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkInteractorStyleImage.h>

//opencv includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;

#include "dsxCube.h"

class dsxXrayImage
{
public:
	dsxXrayImage();
	~dsxXrayImage();
    Mat currentImage;
private:
    std::vector<Mat> imageStack;
    int currentFrame = 0;
    dsxCube parameters;
public:
    /*static xrayImage * New()
     {
     xrayImage * image = new xrayImage();
     return image;
     }*/
    void initialize(char * imageName,char * cubeName, int startFrame = 0);
    void toImageData( vtkImageData * im);
    double getCubeParameterAt(int i);
};


#endif
