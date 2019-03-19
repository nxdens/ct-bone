#include "dsxTiffMovie.h"

dsxTiffMovie::dsxTiffMovie()
{
}

dsxTiffMovie::~dsxTiffMovie()
{
}

void dsxTiffMovie::setImageViewer(vtkImageViewer2* imageViewer)
{
    m_maxSlices = imageViewer->GetSliceMax();
    m_minSlices = imageViewer->GetSliceMin();
    m_ImageViewer = imageViewer;
}

void dsxTiffMovie::moveForward()
{
    if(m_currentSlice < m_maxSlices)
    {
        m_currentSlice += 1;
        cout<< m_currentSlice << endl;
        m_ImageViewer->SetSlice(m_currentSlice);
        m_ImageViewer->Render();
    }
}

void dsxTiffMovie::moveBackwards()
{
    if(m_currentSlice > m_minSlices)
    {
        m_currentSlice -= 1;
        cout<< m_currentSlice << endl;
        m_ImageViewer->SetSlice(m_currentSlice);
        m_ImageViewer->Render();
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


