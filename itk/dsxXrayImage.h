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
#include <vtkImageSliceMapper.h>
#include <vtkImageSlice.h>

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

    void Initialize(std::string imageName, std::string cubeName, int startFrame = 0);

    int GetNumberOfFrames(void);

    double GetCubeParameter(int index);

    double GetCubeParameterS2d(void);
    double GetCubeParameterX(void);
    double GetCubeParameterY(void);
    double GetCubeParameterZ(void);
    double GetCubeParameterRoll(void);
    double GetCubeParameterPitch(void);
    double GetCubeParameterYaw(void);
    
    vtkImageData * GetVtkImageData(void);
    vtkSmartPointer<vtkImageSlice> GetVtkImageSlice(int frameNumber);
    void GetVtkImageData2D(vtkSmartPointer<vtkImageData> imageData2D, int frameNumber);

    //Mat currentImage;
private:
    void TranferImageStackToVtkImageData3D(vtkImageData * imageData3D);

    std::vector<Mat> m_imageStack;
    int m_currentFrame = 0;
    dsxCube m_cube;
    vtkSmartPointer<vtkImageData> m_vtkImageData3D;
};


#endif
