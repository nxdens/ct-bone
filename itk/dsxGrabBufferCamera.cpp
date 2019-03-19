#include "dsxGrabBufferCamera.h"

dsxGrabBufferCamera::dsxGrabBufferCamera()
{
}

dsxGrabBufferCamera::~dsxGrabBufferCamera()
{
}

void dsxGrabBufferCamera::SetWindow(vtkRenderWindow* ctWindow, vtkImageViewer2* xrayViewer, vtkImageData* xrayData, vtkRenderer* otherRenderer,std::vector<double> differences,dsxBone * tDsxBone)
{
    trackballActorStyle = vtkSmartPointer<dsxGrabBufferActor>::New();
    trackballActorStyle->SetWindow(ctWindow,xrayViewer,xrayData,otherRenderer,differences,tDsxBone);
}

void dsxGrabBufferCamera::OnKeyPress() 
{
    std::string key = this->Interactor->GetKeySym();
    if(key == "s")
    {
        //Switch from dsxGrabBufferCamera to dsxGrabBufferActor
        cout <<"okay dsxGrabBufferCamera::OnKeyPress()" << endl;
        this->Interactor->SetInteractorStyle(trackballActorStyle);
    }
    vtkInteractorStyleTrackballCamera::OnKeyPress();
}

