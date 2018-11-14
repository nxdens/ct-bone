//itk includes
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRGBPixel.h" //need this to use any sort of rgb 
#include <itkImageToVTKImageFilter.h>
#include "itkVTKImageToImageFilter.h"
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

//opencv includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

//c and cpp standards
#include "QuickView.h"
#include <stdio.h>
#include <fstream>
#include <vector>
#include <chrono>
#define PI 3.14159265
using namespace cv;
/*
	ToDo:
	parameterize the volume setup and declaration so that we can have multiple volume objects probably create a new wrapper class to make it cleaner since most of the data is the same
	clean up variable names in class definitions
	multiple actors 
	animations
*/
class cube{//this gets the parameters from the document with the calibration cube optimization information should be used to get the roll pitch and yaw of each camera then use that to get the relative position of the offset camera
	public:
		std::vector<double> cubeParameters;//s2d,x,y,z,roll,pitch,yaw,ppx,ppy,error^2

		void findParams(char * filename)
		{
			ifstream cubeFile;
			cubeFile.open(filename);
			std::string currentString;
			std::string previousString;
			std::string S2D;
			//parses file to get the parameters for all the values

			//finds the start of the parameters
			while(getline(cubeFile,currentString))
			{
				if(currentString.find("S2D") != std::string::npos)
				{
					getline(cubeFile,currentString);
					S2D = currentString.substr(0,currentString.find("	"));
					break;
				}
			}
			//skips until it is the last line of parameters
			while(getline(cubeFile,currentString) && currentString.find(S2D) != std::string::npos)
			{
				previousString = currentString;
			}
			
			std::string::size_type previousString_pos = 0, pos = 0;
			//grabs the last row of data 
			while((pos = previousString.find("	", pos)) != std::string::npos)
		    {
		        std::string substring(previousString.substr(previousString_pos, pos-previousString_pos) );
		        cubeParameters.push_back(stod(substring));
		        previousString_pos = ++pos;
		    }
		    cubeFile.close();
		}
};
//this whole class needs cleaning up
class grabBufferActor : public vtkInteractorStyleTrackballActor
{
   public:
		static grabBufferActor * New();
		vtkTypeMacro(grabBufferActor, vtkInteractorStyleTrackballActor);
		vtkSmartPointer<vtkImageData> xrayImageData;//used only to blend images should try to find a better wa to do this so that we can open tiff files
		std::vector<double> cubeParametersDifs;
		vtkSmartPointer<vtkRenderer> otherRenderer;// give both window the other window and be able to switch on and off on screen rendering of the other with one 

   protected:
      	vtkSmartPointer<vtkRenderWindow> ctRenderWindow;
      	vtkSmartPointer<vtkImageBlend> xrayCTImageBlender;
      	vtkSmartPointer<vtkVolume> ctProp; //eventually change this to be an array for multiple objects
      	int height;
      	int width;
    	const char rgb = 3;
    	int scale = 1;
  		int delta = 0;
  		int ddepth = CV_16S;
    	unsigned char* ctPixelDataMatrix;
		unsigned char* xrayPixelMatrix;
		Mat src_grayXray, grad_xXray, grad_yXray;
		Mat abs_grad_xXray, abs_grad_yXray;
		vtkSmartPointer<vtkImageViewer2> xrayImageViewer;

   public:
		void SetWindow(vtkRenderWindow* ctWindow, vtkImageViewer2* xrayViewer, vtkImageData* xrayData, vtkRenderer* offset,std::vector<double> differences,vtkVolume * ctVolume )
		{
			ctRenderWindow = ctWindow;
			int* dim = ctWindow->GetSize();
			height = dim[0]-1;
			width = dim[1]-1;
			//cout <<"height: " << height+1 <<"\t width = " << width+1 << endl;
			ctPixelDataMatrix = new unsigned char[dim[0]*dim[1]*3+1];
			xrayImageData = xrayData;
			xrayImageViewer = xrayViewer;
			otherRenderer = offset;
			cubeParametersDifs = differences;
			ctProp = ctVolume;
			ctVolume->RotateX(1);
			setXray();
		}
		void setImageData(vtkImageData * im)// eventually switch this to window to image filter instaed of this jank
		{
			vtkSmartPointer<vtkUnsignedCharArray> ar = vtkSmartPointer<vtkUnsignedCharArray>::New();
			ar->SetArray(ctPixelDataMatrix,(height+1)*(width+1),1);
			im->SetDimensions(height+1,width+1,1);
			im->AllocateScalars(VTK_UNSIGNED_CHAR,3);
			unsigned char* currentPixel;
			for(int i =0;i<=width;i++)
			{
				for(int j = 0 ; j<=height;j++)
				{
				   currentPixel = static_cast<unsigned char*>(im->GetScalarPointer(i,j,0));
				   if(ctPixelDataMatrix[(j*(width+1)+i)*3] != NULL)
				   {
				   	char intensity =ctPixelDataMatrix[(j*(width+1)+i)*3]*2;
					//cout << ctPixelDataMatrix[(j*(width+1)+i)*3] << " ";
					//set different values to 0 to give different color to the image 
					//still need to figure out how to add opacity to the image
					currentPixel[0] = intensity;
					currentPixel[1] = 0;//ctPixelDataMatrix[(j*(width+1)+i)*3];
					currentPixel[2] = 0;//ctPixelDataMatrix[(j*(width+1)+i)*3];
				   }
				   else
				   {
					currentPixel[0] = 0;
					currentPixel[1] = 0;
					currentPixel[2] = 0;
				   }
				}
			}
			im->Modified();
			//im->SetInputData(ctPixelDataMatrix);//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaahhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh why doesnt this just work
		}
		void startTimer()
		{
			this->Interactor->CreateRepeatingTimer(40);
		}
		void stopTimer()
		{
			this->Interactor->DestroyTimer();
		}
		void getMetric(unsigned char * matrx)//this blocks everthing but mostly because of the imageviewer2 just get rid of the visualization to fix that 
		{
			//gradient for the ct

			//pretty slow mostly the cv kernel stuff but also needs to be graphics card accelerated
			Mat m(height+1, width+1, CV_8UC3,matrx);// this works so we can now use opencv
			Mat src_grayCT, grad_xCT, grad_yCT;
			Mat abs_grad_xCT, abs_grad_yCT;
			cvtColor( m, src_grayCT, CV_BGR2GRAY );
			Sobel(src_grayCT,grad_xCT,ddepth,1,0,3,scale,delta, BORDER_DEFAULT);
			convertScaleAbs( grad_xCT, abs_grad_xCT );
			Sobel(src_grayCT,grad_yCT,ddepth,0,1,3,scale,delta, BORDER_DEFAULT);
			convertScaleAbs( grad_yCT, abs_grad_yCT );//do dot product on the grad_x and y not the abs ones since you want the negative values

			//Calculates gradient correlation
			Mat y,x,xx,yy;
			Mat x2,y2;
			y = grad_yXray.mul(grad_yCT);
			x = grad_xXray.mul(grad_xCT);
			double sumY = sum(y)[0];
			double sumX = sum(x)[0];
			xx = grad_xCT.mul(grad_xCT);
			yy = grad_yCT.mul(grad_yCT);
			x2 = grad_xXray.mul(grad_xXray);
			y2 = grad_yXray.mul(grad_yXray);
			double SS1 = sum(yy)[0] + sum(xx)[0];
			double SQ1 = std::sqrt(SS1);
			double SS2 = sum(y2)[0] + sum(x2)[0];
			double SQ2 = std::sqrt(SS2);

			//use to show that image correlated with itself is 1
			/*
			y = grad_yXray.mul(grad_yXray);
			x = grad_xXray.mul(grad_xXray);
			double sumY = sum(y)[0];
			double sumX = sum(x)[0];
			xx = grad_xXray.mul(grad_xXray);
			yy = grad_yXray.mul(grad_yXray);
			x2 = grad_xXray.mul(grad_xXray);
			y2 = grad_yXray.mul(grad_yXray);
			double SS1 = sum(yy)[0] + sum(xx)[0];
			double SQ1 = std::sqrt(SS1);
			double SS2 = sum(y2)[0] + sum(x2)[0];
			double SQ2 = std::sqrt(SS2);
			*/

			cout << std::setprecision(4) <<( sumY + sumX)/(SQ1*SQ2)<< endl;
		}
		void visualizeBuffer()
		{
			vtkSmartPointer<vtkImageData> im = vtkSmartPointer<vtkImageData>::New();
			setImageData(im);

			xrayCTImageBlender = vtkSmartPointer<vtkImageBlend>::New();
			//xrayCTImageBlender->SetBlendModeToCompound();//this doesnt seem to do anything
			xrayCTImageBlender->AddInputData(im);
			xrayCTImageBlender->AddInputData(xrayImageData);

			xrayCTImageBlender->SetOpacity(0, .5);
			xrayCTImageBlender->SetOpacity(1,.5);
			xrayCTImageBlender->Update();
			
			xrayImageViewer->SetInputData(xrayCTImageBlender->GetOutput());
			xrayImageViewer->GetRenderer()->ResetCamera();
			xrayImageViewer->Render();

			getMetric(ctPixelDataMatrix);
		 
		}
   
		void captureBuffer()
		{
			ctPixelDataMatrix = ctRenderWindow->GetPixelData(0,0,height,width,true);
			//superimposes and generates a metric
			visualizeBuffer();
		}
		void setXray()
		{
			
			int *dims = xrayImageViewer->GetSize();
			xrayPixelMatrix = xrayImageViewer->GetRenderWindow()->GetPixelData(0,0,dims[0]-1,dims[1]-1,true);
			
			//gradient for the xray
			Mat m3(xrayImageViewer->GetSize()[0], xrayImageViewer->GetSize()[1], CV_8UC3,xrayPixelMatrix);
			Mat m2;
			//resize so they have the same number of pixels
			resize(m3,m2,cv::Size(),500.0/xrayImageViewer->GetSize()[0],500.0/xrayImageViewer->GetSize()[0]);
			//sets values for gradients of the xray images so it doesnt need to be recomputed
			cvtColor( m2, src_grayXray, CV_BGR2GRAY );
			Sobel(src_grayXray,grad_xXray,ddepth,1,0,3,scale,delta, BORDER_DEFAULT);
			convertScaleAbs( grad_xXray, abs_grad_xXray );
			Sobel(src_grayXray,grad_yXray,ddepth,0,1,3,scale,delta, BORDER_DEFAULT);
			convertScaleAbs( grad_yXray, abs_grad_yXray );
		}
		
		virtual void OnKeyPress()
		{
			//get the key pressed from the interactor
			//the interactor is some sort of event sniffer that looks for specific events and the style changes only what type of handler is used for each event
			std::string key = this->Interactor->GetKeySym();
			//cout << "Pressed: "<< key << endl;
			if(key == "c")
			{	
				//this gets the frame buffer from the render window and passes it to opencv to generate a metric
				captureBuffer();
			}
			if(key == "s")
			{
				//must be set here or the program won't work -\_(-_-)_/-
				cout <<"okay" << endl;
				setXray();
			}
			if(key == "d")
			{
				startTimer();
			}

			vtkInteractorStyleTrackballActor::OnKeyPress();
		}


};

vtkStandardNewMacro(grabBufferActor);

class grabBufferCamera : public vtkInteractorStyleTrackballCamera
{
   public:
		static grabBufferCamera * New();
		vtkTypeMacro(grabBufferCamera, vtkInteractorStyleTrackballCamera);
		vtkSmartPointer<grabBufferActor> trackballActorStyle;
   public:
		void SetWindow(vtkRenderWindow* ctWindow, vtkImageViewer2* xrayViewer, vtkImageData* xrayData, vtkRenderer* otherRenderer,std::vector<double> differences,vtkVolume * ctVolume)
		{
			ctWindow->Render();
			trackballActorStyle = vtkSmartPointer<grabBufferActor>::New();
			trackballActorStyle->SetWindow(ctWindow,xrayViewer,xrayData,otherRenderer,differences,ctVolume);
			//_renderWindow->GetInteractor()->SetInteractorStyle(trackballActorStyle);
		}
   protected:
		virtual void OnKeyPress()
		{
			std::string key = this->Interactor->GetKeySym();
			if(key == "s")
			{
				this->Interactor->SetInteractorStyle(trackballActorStyle);
			}
			vtkInteractorStyleTrackballCamera::OnKeyPress();
		}

};

vtkStandardNewMacro(grabBufferCamera);

//this just reRenders the other renderer from the given window
class reRenderer : public vtkCommand
{
public:
	static reRenderer * New()
	{
		reRenderer * commander = new reRenderer();
		return commander;
	}
	virtual void Execute(vtkObject *caller, unsigned long eventId, void * vtkNotUsed(callData))
	{
		//cout << "modded" << endl;
		//this is jagged when attached to the volume modified and i suspect that it is due to the fact that it gets called too often
      	otherRenderer->Render();//this is a really slow action for some reason
	}

	vtkSmartPointer<vtkRenderWindow> otherRenderer;
};

class volumeSpinner :public vtkCommand
{
	public:
		static volumeSpinner * New()
		{
			volumeSpinner * spinner = new volumeSpinner();
			return spinner;
		}
		virtual void Execute(vtkObject * caller, unsigned long eventId, void * vtkNotUsed(callData))
		{
			if(eventId == vtkCommand::TimerEvent)
			{
				if(eventId != vtkCommand::LeftButtonPressEvent)
				{
					ctVolume->RotateX(2);//this rotates around the world axis
				}
				ctInlineWindow->Render();
				ctOffsetWindow->Render();
			}
		}

		vtkSmartPointer<vtkVolume> ctVolume;
		vtkSmartPointer<vtkRenderWindow> ctInlineWindow;
		vtkSmartPointer<vtkRenderWindow> ctOffsetWindow;
};

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
	//opencv setup to read tiff images will eventually use this once we can superimpose the ct bone onto the window in real time
	std::vector<Mat> offsetStack;
	imreadmulti("lstep3_Cam_Offset_Cine1.cine_DistCorr.tif",offsetStack);
	std::vector<Mat> inlineStack;
	imreadmulti("lstep3_Cam_Inline_Cine1.cine_DistCorr.tif",inlineStack);
	imshow("Offset",inlineStack[25]);//need to get open gl so we can update the window in real time
	//setOpenGlContext("thing");//crashes program if opencv is not compiled correctly
//end opencv tif reader*/

//start cube parameter setup
	//opens the text files to read the offsets fro the two camera parameters
	cube inlineCube;
	inlineCube.findParams("cube_b_Cam_Inline_Cine1.cine_DistCorr.tif.txt");
	cube offsetCube;
	offsetCube.findParams("cube_b_Cam_Offset_Cine1.cine_DistCorr.tif.txt");
	//get the relative position from the inline camera for the offset camera
	std::vector<double> cubeParameterDifferences;// x, y, z, roll, pitch, yaw offset respectively but vtk gives output in pitch yaw roll
	std::vector<double> oppositeCubeParameterDifferences;
	//calculate the differences between the parameters gets the positive (cubeParameterDifferences) and the negative (odif)
	for(int i = 1; i< 7;i++)
	{
		cubeParameterDifferences.push_back(inlineCube.cubeParameters.at(i) - offsetCube.cubeParameters.at(i));
		oppositeCubeParameterDifferences.push_back(-(inlineCube.cubeParameters.at(i) - offsetCube.cubeParameters.at(i)));
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
	xrayImageViewer->SetInputConnection(flipY->GetOutputPort());
	xrayImageViewer->GetRenderer()->ResetCamera();

	xrayImageViewer->SetupInteractor(xrayWindowInteractor);
	xrayWindowInteractor->Initialize();
//end setup first xray view*/

//start important 3d render object declarations
	//would need to get the volume properties parameterized for multiple bones
	std::vector<vtkVolume*> ctVolumes;
	vtkSmartPointer<vtkVolume> ctVolume = vtkSmartPointer<vtkVolume>::New();
	vtkSmartPointer<vtkSmartVolumeMapper> ctVolummeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();

	std::vector<vtkImageData*> ctImageDataVector;
	vtkSmartPointer<vtkImageData> ctImageData = vtkSmartPointer<vtkImageData>::New();
	ctImageDataVector.push_back(ctImageData);
	//setup the main 3d window environment 
	vtkSmartPointer<vtkRenderWindow> ctInlineWindow = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderer> ctInlineRenderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> ctInlineInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<grabBufferCamera> ctInlineInteractorStyle = vtkSmartPointer<grabBufferCamera>::New();
	//create another render window to get the second view need to set the camera so that it matches the parameters of the xray maybe I should just make a header file to automate most of this
	vtkSmartPointer<vtkRenderer> ctOffsetRenderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> ctOffsetWindow = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> ctOffsetInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<grabBufferCamera> ctOffsetInteractorStyle = vtkSmartPointer<grabBufferCamera>::New();

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
	ctInlineRenderer->GetActiveCamera()->SetViewAngle(2*atan(.5*.79*500/inlineCube.cubeParameters[0]) * 180 /PI);
	//sets the position of the camera and converts pixel
	double cameraPosition[3];
	cameraPosition[0] = inlineCube.cubeParameters[1]/.79; //.79 is a constant for cm/pixel
	cameraPosition[1] = inlineCube.cubeParameters[3]/.79; // vtk y, camera z
	cameraPosition[2] = inlineCube.cubeParameters[2]/.79; //vtk z, camera y
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
	ctOffsetRenderer->GetActiveCamera()->SetViewAngle(2*atan(.5*.79*500/offsetCube.cubeParameters[0]) * 180/ PI);
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
	ctInlineInteractorStyle->SetWindow(ctInlineWindow,xrayImageViewer,xrayImageViewer->GetInput(),ctOffsetRenderer,cubeParameterDifferences,ctVolume);
	ctOffsetInteractorStyle->SetWindow(ctOffsetWindow,xrayImageViewer,xrayImageViewer->GetInput(),ctInlineRenderer,oppositeCubeParameterDifferences,ctVolume);
	
	//adds a timer event to move the bone independently and update the window
	//the event should be stopped when the left mouse button is pressed
	vtkSmartPointer<reRenderer> inlineToOffset = vtkSmartPointer<reRenderer>::New();
	inlineToOffset->otherRenderer = ctOffsetWindow;
	ctInlineRenderer->AddObserver(vtkCommand::ModifiedEvent,inlineToOffset);

	vtkSmartPointer<volumeSpinner> ctVolumeSpinner = vtkSmartPointer<volumeSpinner>::New();
	ctVolumeSpinner->ctVolume = ctVolume;
	ctVolumeSpinner->ctInlineWindow = ctInlineWindow;
	ctVolumeSpinner->ctOffsetWindow = ctOffsetWindow;
	//ctInlineInteractor->CreateRepeatingTimer(40);//testing how timer events work
	ctInlineInteractor->AddObserver(vtkCommand::TimerEvent,ctVolumeSpinner );

	vtkSmartPointer<reRenderer> offsetToInline = vtkSmartPointer<reRenderer>::New();
	offsetToInline->otherRenderer = ctInlineWindow;
	ctOffsetRenderer->AddObserver(vtkCommand::ModifiedEvent,offsetToInline);

	ctOffsetRenderer->Render();
	ctInlineRenderer->Render();
//end interactor style setup*/
	//this approach works its pretty fast for now if the superposition is tied to a threaded event rather than keyboard shortcuts
	ctOffsetInteractor->Start();// to sync the cameras just make something that can get the camera position of the other camera and add the offset from the inline camera or whatever

	return 0;
}