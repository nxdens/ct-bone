// Biodinamics Lab DSX Optimization Program
// Linghai Wang and George Stetten
// Copyright 2018

// Top-Level Optimizer Project for Joint Kinematics using DSX 

//itk includes
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRGBPixel.h" //need this to use any sort of rgb
#include <itkImageToVTKImageFilter.h>
//#include "itkVTKImageToImageFilter.h"     THIS DIDN'T SEEM TO BE NEEDED AND WAS PREVENTING COMPILATION
#include "itkThresholdImageFilter.h"

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

//opencv includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

//c and cpp standards
#include "QuickView.h"
#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#define PI 3.14159265
using namespace cv;

// dsx includes
#include "dsxCube.h"
#include "dsxGUI.h"
#include "dsxImageSequence.h"
#include "dsxMetric.h"
#include "dsxOptimizer.h"
#include "dsxTiffMovie.h"
#include "dsxGrabBufferActor.h"
#include "dsxGrabBufferCamera.h"
#include "dsxReRenderer.h"
#include "dsxVolumeSpinner.h"
#include "dsxXrayImage.h"

/*
	ToDo:
	parameterize the volume setup and declaration so that we can have multiple volume objects probably create a new wrapper class to make it cleaner since most of the data is the same
	clean up variable names in class definitions
	multiple actors
	animations
 */

vtkStandardNewMacro(dsxGrabBufferActor);
vtkStandardNewMacro(dsxGrabBufferCamera);
vtkStandardNewMacro(dsxTiffMovie);

int main(int argc, char *argv[])
{
    
    //usage statements
    if(argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " [inputImageFile]" << std::endl;
        return EXIT_FAILURE;
    }
    cout << "Press s twice before using c to generate a metric\n";
    
    //start opencv tif reader
    /*//opencv setup to read tiff images will eventually use this once we can superimpose the ct bone onto the window in real time
     std::vector<Mat> offsetStack;
     imreadmulti("lstep3_Cam_Offset_Cine1.cine_DistCorr.tif",offsetStack);
     std::vector<Mat> inlineStack;
     imreadmulti("lstep3_Cam_Inline_Cine1.cine_DistCorr.tif",inlineStack);
     imshow("Offset",inlineStack[25]);//need to get open gl so we can update the window in real time
     //setOpenGlContext("thing");//crashes program if opencv is not compiled correctly
     //end opencv tif reader*/
    vtkSmartPointer<vtkImageData> inlineImageData = vtkSmartPointer<vtkImageData>::New();
    vtkSmartPointer<vtkImageData> offsetImageData = vtkSmartPointer<vtkImageData>::New();
    dsxXrayImage inlineImage;
    
    char * tempString1 = "lstep3_Cam_Inline_Cine1.cine_DistCorr.tif";
    char * tempString2 = "cube_b_Cam_Inline_Cine1.cine_DistCorr.tif.txt";
    inlineImage.initialize(tempString1,tempString2);\
    inlineImage.toImageData(inlineImageData);
    dsxXrayImage offsetImage;
    offsetImage.initialize("lstep3_Cam_Offset_Cine1.cine_DistCorr.tif","cube_b_Cam_Offset_Cine1.cine_DistCorr.tif.txt");
    offsetImage.toImageData(offsetImageData);
    
    //start cube parameter setup
    //opens the text files to read the offsets fro the two camera parameters
    /*cube inlineCube;
     inlineCube.findParams("cube_b_Cam_Inline_Cine1.cine_DistCorr.tif.txt");
     cube offsetCube;
     offsetCube.findParams("cube_b_Cam_Offset_Cine1.cine_DistCorr.tif.txt");*/
    //get the relative position from the inline camera for the offset camera
    std::vector<double> cubeParameterDifferences;// x, y, z, roll, pitch, yaw offset respectively but vtk gives output in pitch yaw roll
    std::vector<double> oppositeCubeParameterDifferences;
    //calculate the differences between the parameters gets the positive (cubeParameterDifferences) and the negative (odif)
    for(int i = 1; i< 7;i++)
    {
        cubeParameterDifferences.push_back(inlineImage.getCubeParameterAt(i) - offsetImage.getCubeParameterAt(i));
        oppositeCubeParameterDifferences.push_back(-(inlineImage.getCubeParameterAt(i) - offsetImage.getCubeParameterAt(i)));
    }
    //end cube parameter setup
    
    //start itk xray data read
    typedef itk::Image<unsigned char, 2> xrayType;
    typedef itk::ImageFileReader<xrayType> xrayReader;
    
    //reads the xray image
    xrayReader::Pointer xrayReaderObject = xrayReader::New();
    xrayReaderObject->SetFileName("background.png");//eventually this will be a tiff stack
    xrayReaderObject->Update();
    xrayType::Pointer xrayImage = xrayReaderObject->GetOutput();
    
    typedef itk::ImageToVTKImageFilter<xrayType> xrayITKtoVTKFilter;
    //connects the itk reading of the xray image to vtk to display
    xrayITKtoVTKFilter::Pointer xrayITKtoVTK = xrayITKtoVTKFilter::New();
    xrayITKtoVTK->SetInput(xrayImage);
    xrayITKtoVTK->Update();
    //flip along the y axis to correct for the different origins
    vtkSmartPointer<vtkImageFlip> flipY = vtkSmartPointer<vtkImageFlip>::New();
    flipY->SetFilteredAxis(1);
    flipY->SetInputData(xrayITKtoVTK->GetOutput());
    flipY->Update();
    //end itk xray read
    
    //start setup first side xray view
    //creates the display window for the xray
    //setup the side view to see the xray with ct screen overlay
    vtkSmartPointer<vtkRenderWindowInteractor> xrayWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<vtkImageViewer2> xrayImageViewer = vtkSmartPointer<vtkImageViewer2>::New();
    xrayImageViewer->SetInputData(inlineImageData);
    xrayImageViewer->GetRenderer()->ResetCamera();
    vtkSmartPointer<dsxTiffMovie> myInteractorStyle =
    vtkSmartPointer<dsxTiffMovie>::New();
    myInteractorStyle->setImageViewer(xrayImageViewer);
    xrayImageViewer->SetupInteractor(xrayWindowInteractor);
    xrayWindowInteractor->SetInteractorStyle(myInteractorStyle);
    xrayWindowInteractor->Initialize();
    
    xrayImageViewer->SetSlice(25);
    
    vtkSmartPointer<vtkRenderWindowInteractor> xrayWindowInteractor2 = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<vtkImageViewer2> xrayImageViewer2 = vtkSmartPointer<vtkImageViewer2>::New();
    xrayImageViewer2->SetInputData(offsetImageData);
    xrayImageViewer2->GetRenderer()->ResetCamera();
    vtkSmartPointer<dsxTiffMovie> myInteractorStyle2 =
    vtkSmartPointer<dsxTiffMovie>::New();
    myInteractorStyle2->setImageViewer(xrayImageViewer2);
    xrayImageViewer2->SetupInteractor(xrayWindowInteractor2);
    xrayWindowInteractor2->SetInteractorStyle(myInteractorStyle2);
    xrayWindowInteractor2->Initialize();
    
    
    //end setup first xray view*/
    
    //start important 3d render object declarations
    //would need to get the volume properties parameterized for multiple bones
    std::vector<vtkVolume*> ctVolumes;
    vtkSmartPointer<vtkVolume> ctVolume = vtkSmartPointer<vtkVolume>::New();
    vtkSmartPointer<vtkSmartVolumeMapper> ctVolummeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
    
    ctDsxBone = dsxBone::dsxBone(ctVolume);

    std::vector<vtkImageData*> ctImageDataVector;
    vtkSmartPointer<vtkImageData> ctImageData = vtkSmartPointer<vtkImageData>::New();
    ctImageDataVector.push_back(ctImageData);
    //setup the main 3d window environment
    vtkSmartPointer<vtkRenderWindow> ctInlineWindow = vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkRenderer> ctInlineRenderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindowInteractor> ctInlineInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<dsxGrabBufferCamera> ctInlineInteractorStyle = vtkSmartPointer<dsxGrabBufferCamera>::New();
    //create another render window to get the second view need to set the camera so that it matches the parameters of the xray maybe I should just make a header file to automate most of this
    vtkSmartPointer<vtkRenderer> ctOffsetRenderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> ctOffsetWindow = vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkRenderWindowInteractor> ctOffsetInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<dsxGrabBufferCamera> ctOffsetInteractorStyle = vtkSmartPointer<dsxGrabBufferCamera>::New();
    
    //needs this to be able to render the data since default opacity for everything is 0
    vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
    compositeOpacity->AddPoint(99,0.0);
    //just sets everything to 1 can use multiple piecewise function points to threshold certain densities
    compositeOpacity->AddPoint(100.0,1);
    //end 3d object declarations
    
    //start itk raw ct read
    //Read in the raw data file
    typedef itk::Image< unsigned short ,3>	ctRawType;
    typedef itk::ImageFileReader<ctRawType> ctReaderType;
    ctReaderType::Pointer ctRawReader = ctReaderType::New();
    ctRawReader->SetFileName(argv[1]);
    ctRawReader->Update();
    ctRawType::Pointer ctRawImage = ctRawReader->GetOutput();
    //image->Print(std::cout ); //use to print image header data
    //end itk raw ct read
    
    //start of volume setup from itk to vtk
    //filter to send the raw data from itk to vtk
    //typedef of specific type of filter for the xray image type
    typedef itk::ImageToVTKImageFilter<ctRawType> ctITKtoVTKFilter;
    //create a new filter then fass the itk data
    ctITKtoVTKFilter::Pointer ctITKtoVtk = ctITKtoVTKFilter::New();
    ctITKtoVtk->SetInput(ctRawImage);
    ctITKtoVtk->Update();//call this to update the pipeline
    //copy the data from the filter to a mroe vtk friendly data format
    ctImageDataVector[0]->DeepCopy(ctITKtoVtk->GetOutput());//need to do deep copy to reuse the reader object
    vtkSmartPointer<vtkPolyDataAlgorithm> ctPoly = vtkSmartPointer<vtkPolyDataAlgorithm>::New();
    ctPoly->AddInputData(ctImageDataVector[0]);
    
    //set up the volume rendering properties
    //ctVolummeMapper->SetBlendModeToAverageIntensity();
    ctVolummeMapper->SetRequestedRenderModeToGPU();
    //pass the ct volumetric data to the mapper
    ctVolummeMapper->SetInputData(ctImageDataVector[0]);
    //ctVolummeMapper->SetFinalColorWindow(-.75); // this does some weird stuff but in theory should be able to increase brightness
    ctVolume->SetMapper(ctVolummeMapper);
    ctVolume->GetProperty()->SetScalarOpacity(compositeOpacity);
    //end of volume set up*/
    
    //second volume
    ctRawReader = ctReaderType::New();//seems like you need to make a new pointer everytime
    ctRawReader->SetFileName("Lpatellaaxial_74x86x95x16LE.mhd");//set the reader to read a new file
    ctRawReader->Update();
    ctRawImage = ctRawReader->GetOutput();
    ctITKtoVtk->SetInput(ctRawImage);
    ctITKtoVtk->Update();//i think this will work
    vtkSmartPointer<vtkImageData> ctData2 = vtkSmartPointer<vtkImageData>::New();//need this full statement cant just pass the pointer created for some reason probably garbage collection
    ctImageDataVector.push_back(ctData2);
    ctImageDataVector[1]->DeepCopy(ctITKtoVtk->GetOutput());
    vtkSmartPointer<vtkVolume> ctVolume2 = vtkSmartPointer<vtkVolume>::New();
    vtkSmartPointer<vtkSmartVolumeMapper> ctVolummeMapper2 = vtkSmartPointer<vtkSmartVolumeMapper>::New();
    //ctVolummeMapper2->SetBlendModeToAverageIntensity();
    ctVolummeMapper2->SetRequestedRenderModeToGPU();
    ctVolummeMapper2->SetInputData(ctImageDataVector[1]);
    ctVolume2->SetMapper(ctVolummeMapper2);
    ctVolume2->GetProperty()->SetScalarOpacity(compositeOpacity);
    
    //third volume
    ctRawReader = ctReaderType::New();//seems like you need to make a new pointer everytime
    ctRawReader->SetFileName("Ltibiaaxial_119x110x198x16LE.mhd");//set the reader to read a new file
    ctRawReader->Update();
    ctRawImage = ctRawReader->GetOutput();
    ctITKtoVtk->SetInput(ctRawImage);
    ctITKtoVtk->Update();//i think this will work
    vtkSmartPointer<vtkImageData> ctData3 = vtkSmartPointer<vtkImageData>::New();//need this full statement cant just pass the pointer created for some reason probably garbage collection
    ctImageDataVector.push_back(ctData3);
    ctImageDataVector[2]->DeepCopy(ctITKtoVtk->GetOutput());
    vtkSmartPointer<vtkVolume> ctVolume3 = vtkSmartPointer<vtkVolume>::New();
    vtkSmartPointer<vtkSmartVolumeMapper> ctVolummeMapper3 = vtkSmartPointer<vtkSmartVolumeMapper>::New();
    //ctVolummeMapper2->SetBlendModeToAverageIntensity();
    ctVolummeMapper3->SetRequestedRenderModeToGPU();
    ctVolummeMapper3->SetInputData(ctImageDataVector[2]);
    ctVolume3->SetMapper(ctVolummeMapper3);
    ctVolume3->GetProperty()->SetScalarOpacity(compositeOpacity);
    
    //set the positions of the three bones so they look better to start
    ctVolume2->SetPosition(100,100, 0);
    ctVolume3->SetPosition(150, 150, 0);
    
    //Start setting up inline render window
    //ctOffsetWindow->OffScreenRenderingOn();//sets the second view offscreen if we want
    ctInlineWindow->AddRenderer(ctInlineRenderer);
    ctInlineWindow->SetSize(500,500);
    ctInlineWindow->SetWindowName("Inline");
    
    ctInlineInteractor->SetRenderWindow(ctInlineWindow);
    ctInlineInteractor->SetInteractorStyle(ctInlineInteractorStyle);
    
    ctInlineRenderer->AddVolume(ctVolume);
    ctInlineRenderer->AddVolume(ctVolume2);
    ctInlineRenderer->AddVolume(ctVolume3);
    //sets the camera propeties based on the text files for the cameras
    ctInlineRenderer->ResetCamera();
    ctInlineRenderer->GetActiveCamera()->SetViewAngle(2*atan(.5*.79*500/inlineImage.getCubeParameterAt(0)) * 180 /PI);
    //sets the position of the camera and converts pixel
    double cameraPosition[3];
    cameraPosition[0] = inlineImage.getCubeParameterAt(1)/.79; //.79 is a constant for cm/pixel
    cameraPosition[1] = inlineImage.getCubeParameterAt(3)/.79; // vtk y, camera z
    cameraPosition[2] = inlineImage.getCubeParameterAt(2)/.79; //vtk z, camera y
    ctInlineRenderer->GetActiveCamera()->SetPosition(cameraPosition);
    
    //these two calls should update the window after the camera position is changed but the screen is still black until clicked on (which updates it someother way i guess)
    //grabs the orrientation of the inline camera to set the offset camera relative to it
    double * inlineOrrientation = ctInlineRenderer->GetActiveCamera()->GetOrientation();//orrientation is given in pitch,yaw,roll
    //end inline render window setup*/
    
    //start offset renderwindow start up
    //sets up second window for ct data
    ctOffsetWindow->SetSize(500,500);
    ctOffsetWindow->SetWindowName("offset");
    ctOffsetInteractor->SetInteractorStyle(ctOffsetInteractorStyle);
    
    ctOffsetInteractor->SetRenderWindow(ctOffsetWindow);
    ctOffsetRenderer->AddVolume(ctVolume);
    ctOffsetRenderer->AddVolume(ctVolume2);
    ctOffsetRenderer->AddVolume(ctVolume3);
    ctOffsetRenderer->ResetCamera();
    ctOffsetWindow->AddRenderer(ctOffsetRenderer);
    
    //applies the offset from the first camera and sets the viewing angle
    cameraPosition[0] -= cubeParameterDifferences[0]/.79;
    cameraPosition[1] -= cubeParameterDifferences[2]/.79;//vtk y is camera z so swap what we use to offset
    cameraPosition[2] -= cubeParameterDifferences[1]/.79;
    ctOffsetRenderer->GetActiveCamera()->SetViewAngle(2*atan(.5*.79*500/offsetImage.getCubeParameterAt(0)) * 180/ PI);
    ctOffsetRenderer->GetActiveCamera()->SetPosition(cameraPosition);	
    double * offsetOrrientation = ctOffsetRenderer->GetActiveCamera()->GetOrientation();
    //applies the rotation offset from the first camera
    ctOffsetRenderer->GetActiveCamera()->Roll(offsetOrrientation[2] -inlineOrrientation[2] + cubeParameterDifferences[3]);
    ctOffsetRenderer->GetActiveCamera()->Yaw(offsetOrrientation[1] -inlineOrrientation[1] + cubeParameterDifferences[5]);
    ctOffsetRenderer->GetActiveCamera()->Pitch(offsetOrrientation[0]-inlineOrrientation[0] + cubeParameterDifferences[4]);
    //end inline window set up*/
    
    //ctInlineRenderer->Render();//somehow this creates and error
    ctInlineWindow->Render();
    //ctOffsetRenderer->Render();
    ctOffsetWindow->Render();
    //start interactor style setup
    ctInlineInteractor->Initialize();
    ctOffsetInteractor->Initialize();
    ctInlineInteractorStyle->SetWindow(ctInlineWindow,xrayImageViewer,xrayImageViewer->GetInput(),ctOffsetRenderer,cubeParameterDifferences,ctDsxBone);
    ctOffsetInteractorStyle->SetWindow(ctOffsetWindow,xrayImageViewer,xrayImageViewer->GetInput(),ctInlineRenderer,oppositeCubeParameterDifferences,ctDsxBone);
    
    //adds a timer event to move the bone independently and update the window
    //the event should be stopped when the left mouse button is pressed
    vtkSmartPointer<dsxReRenderer> inlineToOffset = vtkSmartPointer<dsxReRenderer>::New();
    inlineToOffset->otherRenderer = ctOffsetWindow;
    ctInlineRenderer->AddObserver(vtkCommand::ModifiedEvent,inlineToOffset);
    
    
    //first spinner
    vtkSmartPointer<dsxVolumeSpinner> ctVolumeSpinner = vtkSmartPointer<dsxVolumeSpinner>::New();
    ctVolumeSpinner->ctVolume = ctVolume;
    ctVolumeSpinner->ctInlineWindow = ctInlineWindow;
    ctVolumeSpinner->ctOffsetWindow = ctOffsetWindow;
    //ctInlineInteractor->CreateRepeatingTimer(40);//testing how timer events work
    ctInlineInteractor->AddObserver(vtkCommand::TimerEvent,ctVolumeSpinner );
    
    vtkSmartPointer<dsxReRenderer> offsetToInline = vtkSmartPointer<dsxReRenderer>::New();
    offsetToInline->otherRenderer = ctInlineWindow;
    ctOffsetRenderer->AddObserver(vtkCommand::ModifiedEvent,offsetToInline);
    
    ctOffsetRenderer->Render();
    ctInlineRenderer->Render();
    //end interactor style setup*/
    //this approach works its pretty fast for now if the superposition is tied to a threaded event rather than keyboard shortcuts
    ctOffsetInteractor->Start();// to sync the cameras just make something that can get the camera position of the other camera and add the offset from the inline camera or whatever
    
    return 0;
}

/*
int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " [inputImageFile]" << std::endl;
        return EXIT_FAILURE;
    }
    cout << "Press s twice before using c to generate a metric\n";
    
    dsxGUI       * gui       = new dsxGUI();
	dsxOptimizer * optimizer = new dsxOptimizer();

	optimizer->Initialize(gui, argc, argv);
//	oldMain(argc, argv);

	while(optimizer->MetricAboveThreshold())
	{
		if (gui->TrackballMoved())
		{
			optimizer->UpdatePoseFromTrackball();
		}
		optimizer->IterateOpitmizer();
	}
	return(1);
}
*/

