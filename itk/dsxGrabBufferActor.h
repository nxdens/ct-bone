#ifndef __DSX_GRABBUFFERACTOR__
#define __DSX_GRABBUFFERACTOR__

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

//c and cpp standards
#include "QuickView.h"
#include <stdio.h>
#include <fstream>
#include <vector>
#include <chrono>

#include "dsxBone.h"
#include "dsxPose.h"
#define PI 3.14159265
using namespace cv;

class dsxGrabBufferActor : public vtkInteractorStyleTrackballActor
{
public:
	dsxGrabBufferActor();
	~dsxGrabBufferActor();

    static dsxGrabBufferActor * New();
    vtkTypeMacro(dsxGrabBufferActor, vtkInteractorStyleTrackballActor);
    vtkSmartPointer<vtkImageData> xrayImageData;//used only to blend images should try to find a better way to do this so that we can open tiff files
    std::vector<double> cubeParametersDifs;
    vtkSmartPointer<vtkRenderer> otherRenderer;// give both window the other window and be able to switch on and off on screen rendering of the other with one
    
protected:
    vtkSmartPointer<vtkRenderWindow> m_ctRenderWindow;
    vtkSmartPointer<vtkImageBlend> m_xrayCTImageBlender;
    vtkSmartPointer<vtkVolume> m_ctProp; //eventually change this to be an array for multiple objects
    int m_height;
    int m_width;
    const char m_rgb = 3;
    int m_scale = 1;
  	int m_delta = 0;
  	int m_ddepth = CV_16S;
    unsigned char * m_ctPixelDataMatrix;
    unsigned char * m_xrayPixelMatrix;
    Mat m_src_grayXray, m_grad_xXray, m_grad_yXray;
    Mat m_abs_grad_xXray, m_abs_grad_yXray;
    vtkSmartPointer<vtkImageViewer2> m_xrayImageViewer;
    dsxBone * m_ActorDsxBone;
public:
    void SetWindow(vtkRenderWindow * ctWindow, vtkImageViewer2 * xrayViewer, vtkImageData * xrayData, vtkRenderer * offset,std::vector<double> differences,dsxBone * tDsxBone );
    void setImageData(vtkImageData * im);// eventually switch this to window to image filter instaed of this jank
    void startTimer();
    void stopTimer();
    void getMetric(unsigned char * matrx);//this blocks everthing but mostly because of the imageviewer2 just get rid of the visualization to fix that
    void visualizeBuffer();
    void captureBuffer();
    void setXray();
    void OnKeyPress() override;
    void resetPose();
};


#endif

