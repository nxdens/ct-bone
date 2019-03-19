#ifndef __DSX_GRABBUFFERCAMERA__
#define __DSX_GRABBUFFERCAMERA__

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

#include "dsxGrabBufferActor.h"

class dsxGrabBufferCamera : public vtkInteractorStyleTrackballCamera
{
public:
    dsxGrabBufferCamera();
    ~dsxGrabBufferCamera();
    static dsxGrabBufferCamera * New();
    vtkTypeMacro(dsxGrabBufferCamera, vtkInteractorStyleTrackballCamera);  //WHY IS THIS NEEDED?
    void SetWindow(vtkRenderWindow* ctWindow, vtkImageViewer2* xrayViewer, vtkImageData* xrayData, vtkRenderer* otherRenderer,std::vector<double> differences,dsxBone * tDsxBone);
protected:
    void OnKeyPress() override;
    vtkSmartPointer<dsxGrabBufferActor> trackballActorStyle;
};

#endif
