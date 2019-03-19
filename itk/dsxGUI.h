#ifndef __DSX_GUI__
#define __DSX_GUI__

#include "vtkImageViewer2.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkTextActor.h"
#include "vtkTexture.h"
#include "vtkImageActor.h"

//#include "dsxProject.h"   //This causes failure because of circular reference; dsxProject.h inlucdes dsxGUI.h
class dsxProject;           //Used this instead; see http://www.informit.com/articles/article.aspx?p=667505

class dsxGUI
{
    friend class dsxProject;
public:
	dsxGUI();
	~dsxGUI();

    void CreateXrayImageViewerInline(vtkImageData * imageData);
    void CreateXrayImageViewerOffset(vtkImageData * imageData);
    void Initialize(dsxProject * project);
    
private:

 vtkSmartPointer<vtkTextActor> AddText(vtkSmartPointer<vtkRenderer> renderer, const char * text,
                                       double positionX, double positionY, int fontSize,
                                       double red, double green, double blue);

    dsxProject * m_project;

    vtkSmartPointer<vtkImageViewer2> m_xrayImageViewerInline;
    vtkSmartPointer<vtkImageViewer2> m_xrayImageViewerOffset;
    
    vtkSmartPointer<vtkImageViewer2> CreateXrayImageViewer(vtkImageData * imageData);
};

#endif
