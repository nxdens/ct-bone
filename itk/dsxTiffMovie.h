#ifndef __DSX_TIFFMOVIE__
#define __DSX_TIFFMOVIE__

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

#include <stdio.h>
#include <fstream>


class dsxTiffMovie : public vtkInteractorStyleImage
{
public:
	dsxTiffMovie();
	~dsxTiffMovie();
    
    static dsxTiffMovie * New();
    vtkTypeMacro(dsxTiffMovie, vtkInteractorStyleImage);
protected:
    vtkImageViewer2 * m_ImageViewer;
    int m_maxSlices = 0;
    int m_minSlices = 0;
    int m_currentSlice = 0;
public:
    void setImageViewer(vtkImageViewer2* imageViewer);
    void moveForward();
    void moveBackwards();
protected:
    void OnKeyDown();
};

#endif
