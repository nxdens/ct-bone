#ifndef __DSX_RERENDERER__
#define __DSX_RERENDERER__

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


//this just reRenders the other renderer from the given window
class dsxReRenderer : public vtkCommand
{
public:
	dsxReRenderer();
	~dsxReRenderer();
    static dsxReRenderer * New()
    {
        dsxReRenderer * commander = new dsxReRenderer();
        return commander;
    }
    void Execute(vtkObject *caller, unsigned long eventId, void * vtkNotUsed(callData));

    vtkSmartPointer<vtkRenderWindow> otherRenderer;
};

#endif
