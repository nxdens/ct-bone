#ifndef __DSX_VOLUMESPINNER__
#define __DSX_VOLUMESPINNER__

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

class dsxVolumeSpinner :public vtkCommand
{
public:
    dsxVolumeSpinner();
    ~dsxVolumeSpinner();
    static dsxVolumeSpinner * New()
    {
        dsxVolumeSpinner * spinner = new dsxVolumeSpinner();
        return spinner;
    }
    void Execute(vtkObject * caller, unsigned long eventId, void * vtkNotUsed(callData));
    
    vtkSmartPointer<vtkVolume> ctVolume;
    vtkSmartPointer<vtkRenderWindow> ctInlineWindow;
    vtkSmartPointer<vtkRenderWindow> ctOffsetWindow;
};

#endif
