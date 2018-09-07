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

//image processing includes
#include "vtkImageGaussianSmooth.h"

//opencv includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include <vtkImageFlip.h>
#include "QuickView.h"
#include <stdio.h>
using namespace cv;

class grabBuffer : public vtkInteractorStyleTrackballCamera
{
   public:
		static grabBuffer * New();
		vtkTypeMacro(grabBuffer, vtkInteractorStyleTrackballCamera);
		unsigned char* matrix;
		unsigned char* back;
		vtkSmartPointer<vtkImageBlend> blend;
		vtkSmartPointer<vtkImageViewer2> imageViewer;
		vtkSmartPointer<vtkImageData> data;
   protected:
      	vtkRenderWindow * _renderWindow;
      	int height;
      	int width;
    	const char rgb = 3;

   public:
		void SetWindow(vtkRenderWindow* window, vtkImageViewer2* viewer, vtkImageData* dat)
		{
			_renderWindow = window;
			int* dim = window->GetSize();
			height = dim[0]-1;
			width = dim[1]-1;
			cout <<"height: " << height+1 <<"\t width = " << width+1 << endl;
			matrix = new unsigned char[dim[0]*dim[1]*3+1];
			data = dat;
			imageViewer = viewer;
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
					//set different values to 0 to give different hue to the image 
					//still need to figure out how to add opacity to the image
					pixel[0] = matrix[(j*(width+1)+i)*3];
					pixel[1] = matrix[(j*(width+1)+i)*3];
					pixel[2] = matrix[(j*(width+1)+i)*3];
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
		void blur(vtkImageData* data)//this blocks everthing but mostly because of the imageviewer2 just get rid of the visualization to fix that 
		{
			int scale = 1;
  			int delta = 0;
  			int ddepth = CV_16S;
			Mat m(height+1, width+1, CV_8UC3,matrix);// this works so we can now use opencv
			Mat src_gray, grad_x, grad_y;
			Mat abs_grad_x, abs_grad_y;
			cvtColor( m, src_gray, CV_BGR2GRAY );
			Sobel(src_gray,grad_x,ddepth,1,0,3,scale,delta, BORDER_DEFAULT);
			convertScaleAbs( grad_x, abs_grad_x );
			Sobel(src_gray,grad_y,ddepth,0,1,3,scale,delta, BORDER_DEFAULT);
			convertScaleAbs( grad_y, abs_grad_y );//do dot product on the grad_x and y not the abs ones since you want the negative values
			Mat m3(imageViewer->GetSize()[0], imageViewer->GetSize()[1], CV_8UC3,back);
			Mat m2;
			resize(m3,m2,cv::Size(),500.0/300,500.0/300);
			Mat src_gray2, grad_x2, grad_y2;
			Mat abs_grad_x2, abs_grad_y2;
			cvtColor( m2, src_gray2, CV_BGR2GRAY );
			Sobel(src_gray2,grad_x2,ddepth,1,0,3,scale,delta, BORDER_DEFAULT);
			convertScaleAbs( grad_x2, abs_grad_x2 );
			Sobel(src_gray2,grad_y2,ddepth,0,1,3,scale,delta, BORDER_DEFAULT);
			convertScaleAbs( grad_y2, abs_grad_y2 );
			//imshow("image", abs_grad_x2);
			//imshow("image2", abs_grad_y2);
			Mat y,x;
			y = grad_y2.mul(grad_y);
			x = grad_x2.mul(grad_x2);
			int sumY = sum(y)[0];
			int sumX = sum(x)[0];
			cout << sumY + sumX<< endl;
			//updateWindow("image");//compile with opengl
			//updateWindow("image2");
			/*vtkSmartPointer<vtkImageGaussianSmooth> smoother = vtkSmartPointer<vtkImageGaussianSmooth>::New();
			smoother->SetInputData(data);
			smoother->Update();*/
			/*vtkSmartPointer<vtkImageViewer2> views = vtkSmartPointer<vtkImageViewer2>::New();
			vtkSmartPointer<vtkRenderWindowInteractor> ion = vtkSmartPointer<vtkRenderWindowInteractor>::New();
			views->SetupInteractor(ion);
			views->SetInputConnection(smoother->GetOutputPort());
			views->GetRenderer()->ResetCamera();
			views->Render();
			ion->Start();*/
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
			blur(im);
			imageViewer->SetInputData(blend->GetOutput());
			imageViewer->GetRenderer()->ResetCamera();
			imageViewer->Render();
		 
		}
   protected:
		void captureBuffer()
		{
			matrix = _renderWindow->GetPixelData(0,0,height,width,true);
			int *dims = imageViewer->GetSize();
			back = imageViewer->GetRenderWindow()->GetPixelData(0,0,dims[0]-1,dims[1]-1,true);
			visualizeBuffer();
		}
		virtual void OnKeyPress()
		{
			vtkRenderWindowInteractor * rwi = this->Interactor;

			std::string key = rwi->GetKeySym();
			//cout << "Pressed: "<< key << endl;
			if(key == "c")
			{
			captureBuffer();
			}
			vtkInteractorStyleTrackballCamera::OnKeyPress();
		}

};

vtkStandardNewMacro(grabBuffer);

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << "inputImageFile" << std::endl;
		return EXIT_FAILURE;
	}

	typedef itk::Image< unsigned short ,3>	ImageType;
	typedef itk::ImageFileReader<ImageType> ReaderType;

	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(argv[1]);
	reader->Update();
	//This doesnt convert the coordinate systems so the image that you get from vtk is flipped in the y from itk 
	ImageType::Pointer image = reader->GetOutput();
    image->Print(std::cout ); 
	
	typedef itk::Image<unsigned char, 2> BackType;
	typedef itk::ImageFileReader<BackType> BackReader;
	std::vector<Mat> stack;
	imreadmulti("lstep3_Cam_Offset_Cine1.cine_DistCorr.tif",stack);
	imshow("thing",stack[0]);
	BackReader::Pointer red = BackReader::New();
	red->SetFileName("background.png");//eventually this will be a tiff stack
	red->Update();

	BackType::Pointer background = red->GetOutput();

	//setup the main 3d window environment
	vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderer> render = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
	vtkSmartPointer<vtkSmartVolumeMapper> mapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
	vtkSmartPointer<vtkVolumeProperty> prop = vtkSmartPointer<vtkVolumeProperty>::New();
	vtkSmartPointer<vtkImageData> data = vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<grabBuffer> style = vtkSmartPointer<grabBuffer>::New();

	//setup the side view to see the xray with ct screen overlay

	typedef itk::ImageToVTKImageFilter<BackType> BackConnector;

	BackConnector::Pointer bConnect = BackConnector::New();
	bConnect->SetInput(background);
	bConnect->Update();

	vtkSmartPointer<vtkImageFlip> flipY = vtkSmartPointer<vtkImageFlip>::New();
	flipY->SetFilteredAxis(1);
	flipY->SetInputData(bConnect->GetOutput());
	flipY->Update();

	vtkSmartPointer<vtkImageViewer2> viewer = vtkSmartPointer<vtkImageViewer2>::New();
	viewer->SetInputConnection(flipY->GetOutputPort());
	viewer->GetRenderer()->ResetCamera();

	vtkSmartPointer<vtkRenderWindowInteractor> interact = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	
	viewer->SetupInteractor(interact);
	interact->Initialize();

	window->AddRenderer(render);
	window->SetSize(500,500);
	interactor->SetInteractorStyle(style);
	//need a better way to get the image data or pass a differen
	style->SetWindow(window,viewer,viewer->GetInput());
	interactor->SetRenderWindow(window);
	window->Render();

	typedef itk::ImageToVTKImageFilter<ImageType> ConnectorType;

	ConnectorType::Pointer connector = ConnectorType::New();
	connector->SetInput(image);
	connector->Update();//call this to update the pipeline

	data->ShallowCopy(connector->GetOutput());

	mapper->SetBlendModeToComposite();
	mapper->SetRequestedRenderModeToGPU();
	mapper->SetInputData(data);

	vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity =
	vtkSmartPointer<vtkPiecewiseFunction>::New();
	compositeOpacity->AddPoint(0.0,0.0);
	compositeOpacity->AddPoint(100.0,1);
	compositeOpacity->AddPoint(255.0,1);
	prop->SetScalarOpacity(compositeOpacity);
	//prop->SetInterpolationTypeToLinear();
	volume->SetMapper(mapper);
	volume->SetProperty(prop);

	render->AddVolume(volume);
	render->ResetCamera();

	window->Render();
	interactor->Start();
	//*fixes* coordinate system problem
	/*vtkSmartPointer<vtkImageFlip> flipY = vtkSmartPointer<vtkImageFlip>::New();
	flipY->SetFilteredAxis(1);
	flipY->SetInputData(connector->GetOutput());
	flipY->Update();

	vtkSmartPointer<vtkImageViewer2> viewer = vtkSmartPointer<vtkImageViewer2>::New();
	viewer->SetInputConnection(flipY->GetOutputPort());
	connector->Update();
	viewer->GetRenderer()->ResetCamera();

	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	
	viewer->SetupInteractor(interactor);
	interactor->Initialize();
	interactor->Start();*/

	return 0;
}