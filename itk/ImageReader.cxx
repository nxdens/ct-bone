#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRGBPixel.h" //need this to use any sort of rgb 

#include <itkImageToVTKImageFilter.h>
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

#include <vtkImageFlip.h>
#include "QuickView.h"
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
	
	

	vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderer> render = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
	vtkSmartPointer<vtkSmartVolumeMapper> mapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
	vtkSmartPointer<vtkVolumeProperty> prop = vtkSmartPointer<vtkVolumeProperty>::New();
	vtkSmartPointer<vtkImageData> data = vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();


	window->AddRenderer(render);
	window->SetSize(500,500);
	interactor->SetInteractorStyle(style);
	interactor->SetRenderWindow(window);
	window->Render();

	typedef itk::ImageToVTKImageFilter<ImageType> ConnectorType;

	ConnectorType::Pointer connector = ConnectorType::New();
	connector->SetInput(image);
	connector->Update();

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