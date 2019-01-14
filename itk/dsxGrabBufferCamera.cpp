#include "dsxGrabBufferCamera.h"

dsxGrabBufferCamera::dsxGrabBufferCamera()
{
}

dsxGrabBufferCamera::~dsxGrabBufferCamera()
{
}

void dsxGrabBufferCamera::SetWindow(vtkRenderWindow* ctWindow, vtkImageViewer2* xrayViewer, vtkImageData* xrayData, vtkRenderer* otherRenderer,std::vector<double> differences,vtkVolume * ctVolume)
{
    ctWindow->Render();
    trackballActorStyle = vtkSmartPointer<dsxGrabBufferActor>::New();
    trackballActorStyle->SetWindow(ctWindow,xrayViewer,xrayData,otherRenderer,differences,ctVolume);
    //_renderWindow->GetInteractor()->SetInteractorStyle(trackballActorStyle);
}

void dsxGrabBufferCamera::OnKeyPress() 
{
    std::string key = this->Interactor->GetKeySym();
    if(key == "s")
    {
        this->Interactor->SetInteractorStyle(trackballActorStyle);
    }
    vtkInteractorStyleTrackballCamera::OnKeyPress();
}

