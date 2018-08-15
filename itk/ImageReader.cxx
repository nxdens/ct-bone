#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRGBPixel.h" //need this to use any sort of rgb 

#include <itkImageToVTKImageFilter.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindowInteractor.h>

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << "inputImageFile" << std::endl;
		return EXIT_FAILURE;
	}

	typedef itk::Image< itk::RGBPixel<unsigned short> ,3>	ImageType;
	typedef itk::ImageFileReader<ImageType> ReaderType;

	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(argv[1]);

	typedef itk::ImageToVTKImageFilter<ImageType> ConnectorType;

	ConnectorType::Pointer connector = ConnectorType::New();
	connector->SetInput(reader->GetOutput());
	
	vtkSmartPointer<vtkImageViewer2> viewer = vtkSmartPointer<vtkImageViewer2>::New();
	viewer->SetInputData(connector->GetOutput());
	connector->Update();
	viewer->GetRenderer()->ResetCamera();

	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	
	viewer->SetupInteractor(interactor);
	interactor->Initialize();
	interactor->Start();

	return 0;
}