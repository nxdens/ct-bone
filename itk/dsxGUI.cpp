#include "dsxGUI.h"
#include "dsxProject.h"

dsxGUI::dsxGUI()
{
}

dsxGUI::~dsxGUI()
{
}

vtkSmartPointer<vtkTextActor> dsxGUI::AddText(vtkSmartPointer<vtkRenderer> renderer, const char * text,
                                              double positionX, double positionY, int fontSize,
                                              double red, double green, double blue)
{
    vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
    textActor->SetInput (text);
    textActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    textActor->SetPosition (positionX, positionY);
    textActor->GetTextProperty()->SetFontSize (fontSize);
    textActor->GetTextProperty()->SetColor (red, green, blue);
    renderer->AddActor2D(textActor);
    return(textActor);
}

void dsxGUI::Initialize(dsxProject * project)
{
    m_project = project;
    std::cout << "Single-Window System.  Close window to proceed to multi-window system\n";

    //Create a window for multiple renderer, and a single interactor.
    //Can I have (do I need) different types of interactors for different viewports?
    //https://stackoverflow.com/questions/36059829/multiple-interaction-in-1-renderwindow
    vtkSmartPointer<vtkRenderWindow> renderWindow =
        vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetSize(700, 700);
    renderWindow->SetWindowName("BoneZone");   //DOES NOT WORK... WHY?
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();

    //Use vtkInteractorStyleTrackballActor instead of vtkInteractorStyleTrackballCamera
    //  to prevent bone and slice from rotating together.  Also without declaring any style,
    //  default has mouse position controlling trackball velocity, which is harder to control.
    vtkSmartPointer<vtkInteractorStyleTrackballActor> interactorStyle = vtkSmartPointer<vtkInteractorStyleTrackballActor>::New();
    renderWindowInteractor->SetInteractorStyle(interactorStyle);

    //Create renderers for the bones...
    //...for inline viewport,...
    vtkSmartPointer<vtkRenderer> inlineBoneRenderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(inlineBoneRenderer);
    inlineBoneRenderer->SetViewport(0.0, 0.5, 0.5, 1.0);  //xmin,ymin,xmax,ymax
    inlineBoneRenderer->AddVolume(m_project->m_dsxBoneFemur->getVolume());
    
    //...and offset viewports.
    vtkSmartPointer<vtkRenderer> offsetBoneRenderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(offsetBoneRenderer);
    offsetBoneRenderer->SetViewport(0.5, 0.5, 1.0, 1.0);  //xmin,ymin,xmax,ymax
    offsetBoneRenderer->AddVolume(m_project->m_dsxBoneFemur->getVolume());

    //Create renderer for the controlpanel viewport.
    vtkSmartPointer<vtkRenderer> controlpanelRenderer = vtkSmartPointer<vtkRenderer>::New();
    controlpanelRenderer->SetBackground(.1,.1,.1);  //dark grey
    renderWindow->AddRenderer(controlpanelRenderer);
    controlpanelRenderer->SetViewport(0.0, 0.0, 1.0, 0.5);  //xmin,ymin,xmax,ymax
    
    //Label Inline and Offset Viewports
    vtkSmartPointer<vtkTextActor> textActorInline = AddText(inlineBoneRenderer, "Inline", .4, .05, 24, 1.0, 0.0, 0.0);
    vtkSmartPointer<vtkTextActor> textActorOffset = AddText(offsetBoneRenderer, "Offset", .4, .05, 24, 1.0, 0.0, 0.0);

    //Add a separate renderer for the background using layers, see
    //  https://vtk.org/Wiki/VTK/Examples/Cxx/Images/BackgroundImage
    //  to display the dsx image at a given frame as 2D VtkImageData...
    int frameNumber = 50;

    //...to the inline viewport...
    vtkSmartPointer<vtkRenderer> inlineDsxRenderer = vtkSmartPointer<vtkRenderer>::New();
    inlineDsxRenderer->InteractiveOff();
    vtkSmartPointer<vtkImageActor> inlineImageActor = vtkSmartPointer<vtkImageActor>::New();
    vtkSmartPointer<vtkImageData> inlineVtkImageData2D = vtkSmartPointer<vtkImageData>::New();
    m_project->m_inlineImage->GetVtkImageData2D(inlineVtkImageData2D, frameNumber);
    inlineImageActor->SetInputData(inlineVtkImageData2D);
    inlineDsxRenderer->AddActor(inlineImageActor);
    renderWindow->AddRenderer(inlineDsxRenderer);
    inlineDsxRenderer->SetViewport(0.0, 0.5, 0.5, 1.0);  //xmin,ymin,xmax,ymax

    //...and the offset viewport.
    vtkSmartPointer<vtkRenderer> offsetDsxRenderer = vtkSmartPointer<vtkRenderer>::New();
    offsetDsxRenderer->InteractiveOff();
    vtkSmartPointer<vtkImageActor> offsetImageActor = vtkSmartPointer<vtkImageActor>::New();
    vtkSmartPointer<vtkImageData> offsetVtkImageData2D = vtkSmartPointer<vtkImageData>::New();
    m_project->m_offsetImage->GetVtkImageData2D(offsetVtkImageData2D, frameNumber);
    offsetImageActor->SetInputData(offsetVtkImageData2D);
    offsetDsxRenderer->AddActor(offsetImageActor);
    renderWindow->AddRenderer(offsetDsxRenderer);
    offsetDsxRenderer->SetViewport(0.5, 0.5, 1.0, 1.0);  //xmin,ymin,xmax,ymax

    //Set up the layers
    inlineDsxRenderer->SetLayer(0);
    offsetDsxRenderer->SetLayer(0);
    inlineBoneRenderer->SetLayer(1);
    offsetBoneRenderer->SetLayer(1);
    renderWindow->SetNumberOfLayers(2);
    
    //Begin rendering
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindow->Render();
    renderWindowInteractor->Start();  //WITHOUT THIS LINE, LINGHAI'S WINDOWS OPEN BUT NOT MINE
}

void dsxGUI::CreateXrayImageViewerInline(vtkImageData * imageData)
{
    m_xrayImageViewerInline = CreateXrayImageViewer(imageData);
}

void dsxGUI::CreateXrayImageViewerOffset(vtkImageData * imageData)
{
    m_xrayImageViewerOffset = CreateXrayImageViewer(imageData);
}

vtkSmartPointer<vtkImageViewer2> dsxGUI::CreateXrayImageViewer(vtkImageData * imageData)
{
    
}
