//itk includes
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRGBPixel.h" //need this to use any sort of rgb 

//all the rendering includes
#include <itkImageToVTKImageFilter.h>
#include "itkVTKImageToImageFilter.h"
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
#include <vtkPropPicker.h>
//image processing includes
#include "vtkImageGaussianSmooth.h"

//opencv includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include <vtkImageFlip.h>
#include "QuickView.h"
#include <stdio.h>
#include <fstream>
#include <vector>
#define PI 3.14159265
using namespace cv;
class cube{//this gets the parameters from the document with the calibration cube optimization information should be used to get the roll pitch and yaw of each camera then use that to get the relative position of the offset camera
	public:
		std::vector<double> params;//s2d,x,y,z,roll,pitch,yaw,ppx,ppy,error^2

		void findParams(char * filename)
		{
			ifstream file;
			file.open(filename);
			std::string str;
			std::string prev;
			std::string s2d;
			//parses file to get the parameters for all the values
			while(getline(file,str))
			{
				if(str.find("S2D") != std::string::npos)
				{
					getline(file,str);
					s2d = str.substr(0,str.find("	"));
					break;
				}
			}
			while(getline(file,str) && str.find(s2d) != std::string::npos)
			{
				prev = str;
			}
			
			std::string::size_type prev_pos = 0, pos = 0;
			while((pos = prev.find("	", pos)) != std::string::npos)
		    {
		        std::string substring(prev.substr(prev_pos, pos-prev_pos) );
		        params.push_back(stod(substring));
		        prev_pos = ++pos;
		    }
		}
};

class bufferGrab : public vtkInteractorStyleTrackballActor
{
   public:
		static bufferGrab * New();
		vtkTypeMacro(bufferGrab, vtkInteractorStyleTrackballActor);
		unsigned char* matrix;
		unsigned char* back;
		vtkSmartPointer<vtkImageBlend> blend;
		vtkSmartPointer<vtkImageViewer2> imageViewer;
		vtkSmartPointer<vtkImageData> data;//used only to blend images should try to find a better wa to do this so that we can open tiff files
		Mat src_gray2, grad_x2, grad_y2;
		Mat abs_grad_x2, abs_grad_y2;
		std::vector<double> difs;
		vtkSmartPointer<vtkVolume> vol;

   protected:
      	vtkSmartPointer<vtkRenderWindow> _renderWindow;
      	vtkSmartPointer<vtkRenderer> otherR;// give both window the other window and be able to switch on and off on screen rendering of the other with one 
      	int height;
      	int width;
    	const char rgb = 3;

   public:
		void SetWindow(vtkRenderWindow* window, vtkImageViewer2* viewer, vtkImageData* dat, vtkRenderer* offset,std::vector<double> dif )
		{
			_renderWindow = window;
			int* dim = window->GetSize();
			height = dim[0]-1;
			width = dim[1]-1;
			//cout <<"height: " << height+1 <<"\t width = " << width+1 << endl;
			matrix = new unsigned char[dim[0]*dim[1]*3+1];
			data = dat;
			imageViewer = viewer;
			otherR = offset;
			difs = dif;
		}
		void setImageData(vtkImageData * im)// eventually switch this to window to image filter instaed of this jank
		{
			vtkSmartPointer<vtkUnsignedCharArray> ar = vtkSmartPointer<vtkUnsignedCharArray>::New();
			ar->SetArray(matrix,(height+1)*(width+1),1);
			im->SetDimensions(height+1,width+1,1);
			im->AllocateScalars(VTK_UNSIGNED_CHAR,3);
			for(int i =0;i<=width;i++)
			{
				for(int j = 0 ; j<=height;j++)
				{
				   unsigned char* pixel = static_cast<unsigned char*>(im->GetScalarPointer(i,j,0));
				   if(matrix[(j*(width+1)+i)*3] != NULL)
				   {
					//cout << matrix[(j*(width+1)+i)*3] << " ";
					//set different values to 0 to give different color to the image 
					//still need to figure out how to add opacity to the image
					pixel[0] = matrix[(j*(width+1)+i)*3];
					pixel[1] = 0;//matrix[(j*(width+1)+i)*3];
					pixel[2] = 0;//matrix[(j*(width+1)+i)*3];
				   }
				   else
				   {
					pixel[0] = 0;
					pixel[1] = 0;
					pixel[2] = 0;
				   }
				}
			}
			im->Modified();
			//im->SetInputData(matrix);//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaahhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh why doesnt this just work
		}
		void getMetric(unsigned char * matrx)//this blocks everthing but mostly because of the imageviewer2 just get rid of the visualization to fix that 
		{
			//gradient for the ct
			int scale = 1;
  			int delta = 0;
  			int ddepth = CV_16S;
			Mat m(height+1, width+1, CV_8UC3,matrx);// this works so we can now use opencv
			Mat src_gray, grad_x, grad_y;
			Mat abs_grad_x, abs_grad_y;
			cvtColor( m, src_gray, CV_BGR2GRAY );
			Sobel(src_gray,grad_x,ddepth,1,0,3,scale,delta, BORDER_DEFAULT);
			convertScaleAbs( grad_x, abs_grad_x );
			Sobel(src_gray,grad_y,ddepth,0,1,3,scale,delta, BORDER_DEFAULT);
			convertScaleAbs( grad_y, abs_grad_y );//do dot product on the grad_x and y not the abs ones since you want the negative values
			
			//imshow("image", m3);
			//imshow("image2", abs_grad_y2);

			//Calculates gradient correlation
			Mat y,x,xx,yy;
			Mat x2,y2;
			y = grad_y2.mul(grad_y);
			x = grad_x2.mul(grad_x);
			double sumY = sum(y)[0];
			double sumX = sum(x)[0];
			xx = grad_x.mul(grad_x);
			yy = grad_y.mul(grad_y);
			x2 = grad_x2.mul(grad_x2);
			y2 = grad_y2.mul(grad_y2);
			double SS1 = sum(yy)[0] + sum(xx)[0];
			double SQ1 = std::sqrt(SS1);
			double SS2 = sum(y2)[0] + sum(x2)[0];
			double SQ2 = std::sqrt(SS2);
			/*
			//use to show that image correlated with itself is 1
			y = grad_y2.mul(grad_y2);
			x = grad_x2.mul(grad_x2);
			double sumY = sum(y)[0];
			double sumX = sum(x)[0];
			xx = grad_x2.mul(grad_x2);
			yy = grad_y2.mul(grad_y2);
			x2 = grad_x2.mul(grad_x2);
			y2 = grad_y2.mul(grad_y2);
			double SS1 = sum(yy)[0] + sum(xx)[0];
			double SQ1 = std::sqrt(SS1);
			double SS2 = sum(y2)[0] + sum(x2)[0];
			double SQ2 = std::sqrt(SS2);
			*/
			cout << std::setprecision(4) <<(sumY + sumX)/(SQ1*SQ2)<< endl;
		}
		void visualizeBuffer()
		{
			vtkSmartPointer<vtkImageData> im = vtkSmartPointer<vtkImageData>::New();
			setImageData(im);

			blend = vtkSmartPointer<vtkImageBlend>::New();
			//blend->SetBlendModeToCompound();
			blend->AddInputData(im);
			blend->AddInputData(data);

			blend->SetOpacity(0, .5);
			blend->SetOpacity(1,.5);
			blend->Update();
			
			imageViewer->SetInputData(blend->GetOutput());
			imageViewer->GetRenderer()->ResetCamera();
			imageViewer->Render();

			getMetric(matrix);
		 
		}
		//unused was originally used to pass information between the two ct windows
		void applyOffset()
		{
			//double pos[3];

			otherR->GetActiveCamera();
		}
   protected:
		void captureBuffer()
		{
			matrix = _renderWindow->GetPixelData(0,0,height,width,true);
			//superimposes and generates a metric
			visualizeBuffer();
		}

		virtual void OnKeyPress()
		{
			vtkRenderWindowInteractor * rwi = this->Interactor;

			std::string key = rwi->GetKeySym();
			//cout << "Pressed: "<< key << endl;
			if(key == "c")
			{	
				//this gets the frame buffer from the render window and passes it to opencv to generate a metric
				//otherR->GetVolumes()->GetNextVolume()->SetOrientation(this->GetDefaultRenderer()->GetVolumes()->GetNextVolume()->GetOrientation());
				captureBuffer();
				otherR->GetRenderWindow()->Render();
				//this->GetDefaultRenderer()->GetRenderWindow()->Render();
			}
			if(key == "s")
			{
				int scale = 1;
	  			int delta = 0;
	  			int ddepth = CV_16S;
				int *dims = imageViewer->GetSize();
				back = imageViewer->GetRenderWindow()->GetPixelData(0,0,dims[0]-1,dims[1]-1,true);
				cout <<"okay" << endl;
				//gradient for the xray
				Mat m3(imageViewer->GetSize()[0], imageViewer->GetSize()[1], CV_8UC3,back);
				Mat m2;
				//resize so they have the same number of pixels
				resize(m3,m2,cv::Size(),500.0/imageViewer->GetSize()[0],500.0/imageViewer->GetSize()[0]);
				//sets values for gradients of the xray images so it doesnt need to be recomputed
				cvtColor( m2, src_gray2, CV_BGR2GRAY );
				Sobel(src_gray2,grad_x2,ddepth,1,0,3,scale,delta, BORDER_DEFAULT);
				convertScaleAbs( grad_x2, abs_grad_x2 );
				Sobel(src_gray2,grad_y2,ddepth,0,1,3,scale,delta, BORDER_DEFAULT);
				convertScaleAbs( grad_y2, abs_grad_y2 );
			}
			vtkInteractorStyleTrackballActor::OnKeyPress();
		}

};

vtkStandardNewMacro(bufferGrab);

class grabBuffer : public vtkInteractorStyleTrackballCamera
{
   public:
		static grabBuffer * New();
		vtkTypeMacro(grabBuffer, vtkInteractorStyleTrackballCamera);
		vtkSmartPointer<bufferGrab> tr;
   protected:
      	vtkSmartPointer<vtkRenderWindow> _renderWindow;
   public:
		void SetWindow(vtkRenderWindow* window, vtkImageViewer2* viewer, vtkImageData* dat, vtkRenderer* offset,std::vector<double> dif)
		{
			_renderWindow = window;
			tr = vtkSmartPointer<bufferGrab>::New();
			tr->SetWindow(window,viewer,dat,offset,dif);
		}
   protected:
		virtual void OnKeyPress()
		{
			vtkRenderWindowInteractor * rwi = this->Interactor;
			std::string key = rwi->GetKeySym();
			if(key == "s")
			{
				_renderWindow->GetInteractor()->SetInteractorStyle(tr);
			}
			vtkInteractorStyleTrackballCamera::OnKeyPress();
		}

};

vtkStandardNewMacro(grabBuffer);



int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " [inputImageFile]" << std::endl;
		return EXIT_FAILURE;
	}
	cout << "Press s twice before using c to generate a metric\n";
	//Read in the raw data file
	typedef itk::Image< unsigned short ,3>	ImageType;
	typedef itk::ImageFileReader<ImageType> ReaderType;

	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(argv[1]);
	reader->Update();
	//This doesnt convert the coordinate systems so the image that you get from vtk is flipped in the y from itk 
	ImageType::Pointer image = reader->GetOutput();
    //image->Print(std::cout ); 
	//opencv setup to read tiff images will eventually use this once we can superimpose the ct bone onto the window in real time
	typedef itk::Image<unsigned char, 2> BackType;
	typedef itk::ImageFileReader<BackType> BackReader;
	std::vector<Mat> stack;
	imreadmulti("lstep3_Cam_Offset_Cine1.cine_DistCorr.tif",stack);
	std::vector<Mat> stack2;
	imreadmulti("lstep3_Cam_Inline_Cine1.cine_DistCorr.tif",stack2);
	imshow("Offset",stack2[25]);//need to get open gl so we can update the window in real time
	//setOpenGlContext("thing");

	//reads the xray image 
	BackReader::Pointer red = BackReader::New();
	red->SetFileName("background.png");//eventually this will be a tiff stack
	red->Update();
	BackType::Pointer background = red->GetOutput();
	//opens the text files to read the offsets fro the two camera parameters
	cube inlineCube;
	inlineCube.findParams("cube_b_Cam_Inline_Cine1.cine_DistCorr.tif.txt");
	cube offsetCube;
	offsetCube.findParams("cube_b_Cam_Offset_Cine1.cine_DistCorr.tif.txt");
	//get the relative position from the inline camera for the offset camera
	std::vector<double> dif;// x, y, z, roll, pitch, yaw offset respectively but vtk gives output in pitch yaw roll
	std::vector<double> odif;
	//calculate the differences between the parameters gets the positive (dif) and the negative (odif)
	for(int i = 1; i< 7;i++)
	{
		dif.push_back(inlineCube.params.at(i) - offsetCube.params.at(i));
		odif.push_back(-(inlineCube.params.at(i) - offsetCube.params.at(i)));
	}

	

	//setup the main 3d window environment 
	vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderer> render = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
	vtkSmartPointer<vtkSmartVolumeMapper> mapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
	vtkSmartPointer<vtkVolumeProperty> prop = vtkSmartPointer<vtkVolumeProperty>::New();
	vtkSmartPointer<vtkImageData> data = vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<grabBuffer> style = vtkSmartPointer<grabBuffer>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> interact = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	//create another render window to get the second view need to set the camera so that it matches the parameters of the xray maybe I should just make a header file to automate most of this
	vtkSmartPointer<vtkRenderer> render2 = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> window2 = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> interact2 = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<grabBuffer> style2 = vtkSmartPointer<grabBuffer>::New();
	//window2->OffScreenRenderingOn();//set the second view offscreen

	//setup the side view to see the xray with ct screen overlay
	
	typedef itk::ImageToVTKImageFilter<BackType> BackConnector;
	//connects the itk reading of the xray image to vtk to display
	BackConnector::Pointer bConnect = BackConnector::New();
	bConnect->SetInput(background);
	bConnect->Update();
	//flip along the y axis to correct for the different origins
	vtkSmartPointer<vtkImageFlip> flipY = vtkSmartPointer<vtkImageFlip>::New();
	flipY->SetFilteredAxis(1);
	flipY->SetInputData(bConnect->GetOutput());
	flipY->Update();
	//creates the display window for the xray
	vtkSmartPointer<vtkImageViewer2> viewer = vtkSmartPointer<vtkImageViewer2>::New();
	viewer->SetInputConnection(flipY->GetOutputPort());
	viewer->GetRenderer()->ResetCamera();

	
	
	viewer->SetupInteractor(interact);
	interact->Initialize();

	window->AddRenderer(render);
	window->SetSize(500,500);
	interactor->SetInteractorStyle(style);
	//need a better way to get the image data or pass a differen
	style->SetWindow(window,viewer,viewer->GetInput(), render2,dif);
	interactor->SetRenderWindow(window);
	window->Render();

	//filter to send the raw data from itk to vtk
	typedef itk::ImageToVTKImageFilter<ImageType> ConnectorType;

	ConnectorType::Pointer connector = ConnectorType::New();
	connector->SetInput(image);
	connector->Update();//call this to update the pipeline

	data->ShallowCopy(connector->GetOutput());

	mapper->SetBlendModeToComposite();
	mapper->SetRequestedRenderModeToGPU();
	mapper->SetInputData(data);
	//needs this to be able to render the data
	vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity =
	vtkSmartPointer<vtkPiecewiseFunction>::New();
	compositeOpacity->AddPoint(0.0,0.0);
	compositeOpacity->AddPoint(100.0,1);
	compositeOpacity->AddPoint(255.0,1);
	prop->SetScalarOpacity(compositeOpacity);
	volume->SetMapper(mapper);
	volume->SetProperty(prop);

	render->AddVolume(volume);
	//sets the camera propeties based on the text files for the cameras
	render->ResetCamera();
	render->GetActiveCamera()->SetViewAngle(2*atan(.5*.79*500/inlineCube.params[0]) * 180 /PI);
	//sets the position of the camera and converts pixel
	double pos[3];
	render->GetActiveCamera()->GetPosition(pos);
	pos[0] = inlineCube.params[1]/.79; //.79 is a constant for cm/pixel
	pos[1] = inlineCube.params[3]/.79; // vtk y, camera z
	pos[2] = inlineCube.params[2]/.79; //vtk z, camera y
	render->GetActiveCamera()->SetPosition(pos);
	double * orr = render->GetActiveCamera()->GetOrientation();//orrientation is given in pitch,yaw,roll
	window->SetWindowName("Inline");
	window->Render();
	//sets up second window for ct data
	window2->SetSize(500,500);
	window2->SetWindowName("offset");
	interact2->SetInteractorStyle(style2);
	style2->SetWindow(window2,viewer,viewer->GetInput(),render,odif);
	interact2->SetRenderWindow(window2);
	render2->AddVolume(volume);
	render2->ResetCamera();
	window2->AddRenderer(render2);

	window2->Render();
	//applies the offset from the first camera and sets the viewing angle
	pos[0] -= dif[0]/.79;
	pos[1] -= dif[2]/.79;//vtk y is camera z so swap what we use to offset
	pos[2] -= dif[1]/.79;
	render2->GetActiveCamera()->SetViewAngle(2*atan(.5*.79*500/offsetCube.params[0]) * 180/ PI);
	render2->GetActiveCamera()->SetPosition(pos);	
	double * orr2 = render2->GetActiveCamera()->GetOrientation();
	//applies the rotation offset from the first camera
	render2->GetActiveCamera()->Roll(orr2[2] -orr[2] + dif[3]);
	render2->GetActiveCamera()->Yaw(orr2[1] -orr[1] + dif[5]);
	render2->GetActiveCamera()->Pitch(orr2[0]-orr[0] + dif[4]);
	//this approach works its pretty fast for now if the superposition is tied to a threaded event rather than keyboard shortcuts
	interact2->Start();// to sync the cameras just make something that can get the camera position of the other camera and add the offset from the inline camera or whatever

	return 0;
}