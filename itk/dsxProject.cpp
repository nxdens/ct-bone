#include "dsxProject.h"

dsxProject::dsxProject()
{
}

dsxProject::~dsxProject()
{
}

void dsxProject::Initialize(dsxGUI * gui, int argc, char *argv[])
{
    //Open the DSX cine x-ray images and their respective cube calibration data files
    //  ...for Inline DSX...
    m_inlineImage = new dsxXrayImage;
    m_inlineImage->Initialize("lstep3_Cam_Inline_Cine1.cine_DistCorr.tif", "cube_b_Cam_Inline_Cine1.cine_DistCorr.tif.txt");

    //   ...and similarly for Offset DSX
    m_offsetImage = new dsxXrayImage;
    m_offsetImage->Initialize("lstep3_Cam_Offset_Cine1.cine_DistCorr.tif", "cube_b_Cam_Offset_Cine1.cine_DistCorr.tif.txt");

    m_dsxBoneFemur = vtkSmartPointer<dsxBone>::New();
    m_dsxBonePatella = vtkSmartPointer<dsxBone>::New();
    m_dsxBoneTibia = vtkSmartPointer<dsxBone>::New();

    m_dsxBoneFemur->Initialize("Lfemur_axial_129x142x177x16LE.mhd");
    m_dsxBonePatella->Initialize("Lpatellaaxial_74x86x95x16LE.mhd");
    m_dsxBoneTibia->Initialize("Ltibiaaxial_119x110x198x16LE.mhd");

    m_GUI = gui;
}

void dsxProject::RunMultiWindowSystem(void)
{
    //From here on is Linghai's multi-window system
    std::cout << "Multi-Window System\n Press s in each trackball window before using, c to generate a metric\n";

    //set the positions of the three bones so they look better to start
    m_dsxBonePatella->setPosition(100,100, 0);
    m_dsxBoneTibia->setPosition(150, 150, 0);

    //Establish transform from the inline camera for the offset camera
    std::vector<double> cubeParameterDifferences;// x, y, z, roll, pitch, yaw offset respectively but vtk gives output in pitch yaw roll
    std::vector<double> oppositeCubeParameterDifferences;
    //Calculate the differences between the parameters gets the positive (cubeParameterDifferences) and the negative (odif)
    for(int i = 1; i< 7;i++)
    {
        cubeParameterDifferences.push_back(m_inlineImage->GetCubeParameter(i) - m_offsetImage->GetCubeParameter(i));
        oppositeCubeParameterDifferences.push_back(-(m_inlineImage->GetCubeParameter(i) - m_offsetImage->GetCubeParameter(i)));
    }
    
   //Create display window for the xray and side view to see the xray with ct screen overlay...
    //  ...for Inline DSX...
//    m_GUI->CreateXrayImageViewerInline(m_inlineImage->GetImageData());
//
    vtkSmartPointer<vtkRenderWindowInteractor> xrayWindowInteractorInline = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<vtkImageViewer2> xrayImageViewerInline = vtkSmartPointer<vtkImageViewer2>::New();
    xrayImageViewerInline->SetInputData(m_inlineImage->GetVtkImageData());
    xrayImageViewerInline->GetRenderer()->ResetCamera();
    vtkSmartPointer<dsxTiffMovie> interactorStyleInline = vtkSmartPointer<dsxTiffMovie>::New();
    interactorStyleInline->setImageViewer(xrayImageViewerInline);
    xrayImageViewerInline->SetupInteractor(xrayWindowInteractorInline);
    xrayWindowInteractorInline->SetInteractorStyle(interactorStyleInline);
    xrayWindowInteractorInline->Initialize();
    xrayImageViewerInline->SetSlice(25);
//
    //  ...and similarly for Offset DSX.
    vtkSmartPointer<vtkRenderWindowInteractor> xrayWindowInteractorOffset = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<vtkImageViewer2> xrayImageViewerOffset = vtkSmartPointer<vtkImageViewer2>::New();
    xrayImageViewerOffset->SetInputData(m_offsetImage->GetVtkImageData());
    xrayImageViewerOffset->GetRenderer()->ResetCamera();
    vtkSmartPointer<dsxTiffMovie> interactorStyleOffset = vtkSmartPointer<dsxTiffMovie>::New();
    interactorStyleOffset->setImageViewer(xrayImageViewerOffset);
    xrayImageViewerOffset->SetupInteractor(xrayWindowInteractorOffset);
    xrayWindowInteractorOffset->SetInteractorStyle(interactorStyleOffset);
    xrayWindowInteractorOffset->Initialize();
    xrayImageViewerOffset->SetSlice(25);
//
    //setup the main 3d window environment
    vtkSmartPointer<vtkRenderWindow> ctInlineWindow = vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkRenderer> ctInlineRenderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindowInteractor> ctInlineInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<dsxGrabBufferCamera> ctInlineInteractorStyle = vtkSmartPointer<dsxGrabBufferCamera>::New();
    
    //create another render window to get the second view need to set the camera so that it matches the parameters of the xray maybe I should just make a header file to automate most of this
    vtkSmartPointer<vtkRenderWindow> ctOffsetWindow = vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkRenderer> ctOffsetRenderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindowInteractor> ctOffsetInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<dsxGrabBufferCamera> ctOffsetInteractorStyle = vtkSmartPointer<dsxGrabBufferCamera>::New();

    //Start setting up inline render window
    //ctOffsetWindow->OffScreenRenderingOn();//sets the second view offscreen if we want
    ctInlineWindow->AddRenderer(ctInlineRenderer);
    ctInlineWindow->SetSize(500,500);
    ctInlineWindow->SetWindowName("Inline");
    
    ctInlineInteractor->SetRenderWindow(ctInlineWindow);
    ctInlineInteractor->SetInteractorStyle(ctInlineInteractorStyle);
    
    ctInlineRenderer->AddVolume(m_dsxBoneFemur->getVolume());
    ctInlineRenderer->AddVolume(m_dsxBonePatella->getVolume());
    ctInlineRenderer->AddVolume(m_dsxBoneTibia->getVolume());
    //sets the camera propeties based on the text files for the cameras
    ctInlineRenderer->ResetCamera();
    ctInlineRenderer->GetActiveCamera()->SetViewAngle(2*atan(.5*.79*500/m_inlineImage->GetCubeParameter(0)) * 180 /PI);
    //sets the position of the camera and converts pixel
    double cameraPosition[3];
    cameraPosition[0] = m_inlineImage->GetCubeParameterX()/.79; //.79 is a constant for cm/pixel
    cameraPosition[1] = m_inlineImage->GetCubeParameterZ()/.79; // vtk y, camera z
    cameraPosition[2] = m_inlineImage->GetCubeParameterY()/.79; //vtk z, camera y
    ctInlineRenderer->GetActiveCamera()->SetPosition(cameraPosition);
    
    //these two calls should update the window after the camera position is changed but the screen is still black until clicked on (which updates it some other way i guess)
    //grabs the orrientation of the inline camera to set the offset camera relative to it
    double * inlineOrrientation = ctInlineRenderer->GetActiveCamera()->GetOrientation();//orrientation is given in pitch,yaw,roll
    //end inline render window setup
    
    //start offset renderwindow start up
    //sets up second window for ct data
    ctOffsetWindow->SetSize(500,500);
    ctOffsetWindow->SetWindowName("offset");
    ctOffsetInteractor->SetInteractorStyle(ctOffsetInteractorStyle);
    
    ctOffsetInteractor->SetRenderWindow(ctOffsetWindow);
    ctOffsetRenderer->AddVolume(m_dsxBoneFemur->getVolume());
    ctOffsetRenderer->AddVolume(m_dsxBonePatella->getVolume());
    ctOffsetRenderer->AddVolume(m_dsxBoneTibia->getVolume());
    ctOffsetRenderer->ResetCamera();
    ctOffsetWindow->AddRenderer(ctOffsetRenderer);
    
    //applies the offset from the first camera and sets the viewing angle
    cameraPosition[0] -= cubeParameterDifferences[0]/.79;
    cameraPosition[1] -= cubeParameterDifferences[2]/.79;//vtk y is camera z so swap what we use to offset
    cameraPosition[2] -= cubeParameterDifferences[1]/.79;
    ctOffsetRenderer->GetActiveCamera()->SetViewAngle(2*atan(.5*.79*500/m_offsetImage->GetCubeParameterS2d()) * 180/ PI);
    ctOffsetRenderer->GetActiveCamera()->SetPosition(cameraPosition);
    double * offsetOrrientation = ctOffsetRenderer->GetActiveCamera()->GetOrientation();
    //applies the rotation offset from the first camera
    ctOffsetRenderer->GetActiveCamera()->Roll( offsetOrrientation[2] - inlineOrrientation[2] + cubeParameterDifferences[3]);
    ctOffsetRenderer->GetActiveCamera()->Yaw(  offsetOrrientation[1] - inlineOrrientation[1] + cubeParameterDifferences[5]);
    ctOffsetRenderer->GetActiveCamera()->Pitch(offsetOrrientation[0] - inlineOrrientation[0] + cubeParameterDifferences[4]);
    //end offset window set up
    
    //start interactor style setup
    ctInlineInteractor->Initialize();
    ctOffsetInteractor->Initialize();
    ctInlineInteractorStyle->SetWindow(ctInlineWindow,xrayImageViewerInline,xrayImageViewerInline->GetInput(),ctOffsetRenderer,cubeParameterDifferences,m_dsxBoneFemur);
    ctOffsetInteractorStyle->SetWindow(ctOffsetWindow,xrayImageViewerInline,xrayImageViewerInline->GetInput(),ctInlineRenderer,oppositeCubeParameterDifferences,m_dsxBoneFemur);   // WHY IS THIS xrayImageViewerInline and not xrayImageViewerOffset?  ARE BOTH NEEDED?
    
    //adds a timer event to move the bone independently and update the window
    //the event should be stopped when the left mouse button is pressed
    vtkSmartPointer<dsxReRenderer> inlineToOffset = vtkSmartPointer<dsxReRenderer>::New();
    inlineToOffset->otherRenderer = ctOffsetWindow;
    ctInlineRenderer->AddObserver(vtkCommand::ModifiedEvent,inlineToOffset);
/*
   //THIS SECTION NOT NEEDED
    //start important 3d render object declarations
    //would need to get the volume properties parameterized for multiple bones
    vtkSmartPointer<vtkVolume> ctVolume = vtkSmartPointer<vtkVolume>::New();
    
    //first spinner
    vtkSmartPointer<dsxVolumeSpinner> ctVolumeSpinner = vtkSmartPointer<dsxVolumeSpinner>::New();
    ctVolumeSpinner->ctVolume = m_dsxBoneTibia->getVolume();
    ctVolumeSpinner->ctInlineWindow = ctInlineWindow;
    ctVolumeSpinner->ctOffsetWindow = ctOffsetWindow;
    //ctInlineInteractor->CreateRepeatingTimer(40);//testing how timer events work
    ctInlineInteractor->AddObserver(vtkCommand::TimerEvent,ctVolumeSpinner );

*/
    vtkSmartPointer<dsxReRenderer> offsetToInline = vtkSmartPointer<dsxReRenderer>::New();
    offsetToInline->otherRenderer = ctInlineWindow;
    ctOffsetRenderer->AddObserver(vtkCommand::ModifiedEvent,offsetToInline);
    
    ctOffsetRenderer->Render();
    ctInlineRenderer->Render();
    //end interactor style setupdsxGUI->CreateXrayImageViewer
    //this approach works its pretty fast for now if the superposition is tied to a threaded event rather than keyboard shortcuts
    ctOffsetInteractor->Start();// to sync the cameras just make something that can get the camera position of the other camera and add the offset from the inline camera or whatever
}

