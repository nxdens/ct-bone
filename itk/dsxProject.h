#ifndef __DSX_PROJECT__
#define __DSX_PROJECT__

#include <stdio.h>
//itk includes
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRGBPixel.h" //need this to use any sort of rgb
#include <itkImageToVTKImageFilter.h>
//#include "itkVTKImageToImageFilter.h"     THIS DIDN'T SEEM TO BE NEEDED AND WAS PREVENTING COMPILATION
#include "itkThresholdImageFilter.h"

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
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

//opencv includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

//c and cpp standards
#include "QuickView.h"
#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#define PI 3.14159265
using namespace cv;

// dsx includes
#include "dsxCube.h"
//#include "dsxGUI.h"
//#include "dsxImageSequence.h"
//#include "dsxMetric.h"
//#include "dsxOptimizer.h"
#include "dsxTiffMovie.h"
#include "dsxGrabBufferActor.h"
#include "dsxGrabBufferCamera.h"
#include "dsxReRenderer.h"
#include "dsxVolumeSpinner.h"
#include "dsxXrayImage.h"
#include "dsxGUI.h"
#include "dsxBone.h"
#include "dsxPose.h"

//vtkStandardNewMacro(dsxGrabBufferActor);
//vtkStandardNewMacro(dsxGrabBufferCamera);
//vtkStandardNewMacro(dsxTiffMovie);

class dsxProject
{
public:
    dsxProject();
    ~dsxProject();
    void Initialize(dsxGUI * gui, int argc, char *argv[]);
    void RunMultiWindowSystem(void);

    void Temp(void);

    dsxXrayImage * m_inlineImage;
    dsxXrayImage * m_offsetImage;
    
    vtkSmartPointer<dsxBone> m_dsxBoneFemur;
    vtkSmartPointer<dsxBone> m_dsxBonePatella;
    vtkSmartPointer<dsxBone> m_dsxBoneTibia;

private:
    dsxGUI       * m_GUI;
};
#endif

