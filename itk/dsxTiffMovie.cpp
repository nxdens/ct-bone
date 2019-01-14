#include "dsxTiffMovie.h"

dsxTiffMovie::dsxTiffMovie()
{
}

dsxTiffMovie::~dsxTiffMovie()
{
}

void dsxTiffMovie::setImageViewer(vtkImageViewer2* imageViewer)
{
    maxSlices = imageViewer->GetSliceMax();
    minSlices = imageViewer->GetSliceMin();
    _ImageViewer = imageViewer;
}

void dsxTiffMovie::moveForward()
{
    if(currentSlice < maxSlices)
    {
        currentSlice += 1;
        cout<< currentSlice << endl;
        _ImageViewer->SetSlice(currentSlice);
        _ImageViewer->Render();
    }
}

void dsxTiffMovie::moveBackwards()
{
    if(currentSlice > minSlices)
    {
        currentSlice -= 1;
        cout<< currentSlice << endl;
        _ImageViewer->SetSlice(currentSlice);
        _ImageViewer->Render();
    }
}

void dsxTiffMovie::OnKeyDown()
{
    std::string key = this->Interactor->GetKeySym();
    if(key.compare("Up") == 0)
    {
        //cout << "Up arrow key was pressed." << endl;
        moveForward();
    }
    else if(key.compare("Down") == 0)
    {
        //cout << "Down arrow key was pressed." << endl;
        moveBackwards();
    }
    // forward event
    vtkInteractorStyleImage::OnKeyDown();
}


