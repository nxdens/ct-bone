// some standard vtk headers
#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkImageReader2Factory.h>
// headers needed for this example
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageCast.h>
#include <vtkMetaImageWriter.h>
#include <vtkMetaImageReader.h>
#include <vtkImageMandelbrotSource.h>
#include <vtkImageActor.h>
#include <vtkMetaImageReader.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkInteractorStyleTrackballCamera.h>
 
class myVtkInteractorStyleImage : public vtkInteractorStyleImage
{
public:
   static myVtkInteractorStyleImage* New();
   vtkTypeMacro(myVtkInteractorStyleImage, vtkInteractorStyleImage);
 
protected:
   vtkImageViewer2* _ImageViewer;
   int _Slice;
   int _MinSlice;
   int _MaxSlice;
 
public:
   void SetImageViewer(vtkImageViewer2* imageViewer) {
      _ImageViewer = imageViewer;
      _MinSlice = imageViewer->GetSliceMin();
      _MaxSlice = imageViewer->GetSliceMax();
      _Slice = _MinSlice;
      //cout << "Slicer: Min = " << _MinSlice << ", Max = " << _MaxSlice << std::endl;
   }
 
 
 
protected:
   void MoveSliceForward() {
      if(_Slice < _MaxSlice) {
         _Slice += 1;
         //cout << "MoveSliceForward::Slice = " << _Slice << std::endl;
         _ImageViewer->SetSlice(_Slice);
         _ImageViewer->Render();         
      }
   }
 
   void MoveSliceBackward() {
      if(_Slice > _MinSlice) {
         _Slice -= 1;
         //cout << "MoveSliceBackward::Slice = " << _Slice << std::endl;
         _ImageViewer->SetSlice(_Slice);
         _ImageViewer->Render();
      }
   }
 
 
   virtual void OnKeyDown() {
      std::string key = this->GetInteractor()->GetKeySym();
      cout <<_Slice<<endl;
      if(key.compare("Up") == 0) {
         //cout << "Up arrow key was pressed." << endl;
         MoveSliceForward();
      }
      else if(key.compare("Down") == 0) {
         //cout << "Down arrow key was pressed." << endl;
         MoveSliceBackward();
      }
      // forward event
      vtkInteractorStyleImage::OnKeyDown();
   }
 
 
   virtual void OnMouseWheelForward() {
      //std::cout << "Scrolled mouse wheel forward." << std::endl;
      MoveSliceForward();
      // don't forward events, otherwise the image will be zoomed 
      // in case another interactorstyle is used (e.g. trackballstyle, ...)
      // vtkInteractorStyleImage::OnMouseWheelForward();
   }
 
 
   virtual void OnMouseWheelBackward() {
      //std::cout << "Scrolled mouse wheel backward." << std::endl;
      if(_Slice > _MinSlice) {
         MoveSliceBackward();
      }
      // don't forward events, otherwise the image will be zoomed 
      // in case another interactorstyle is used (e.g. trackballstyle, ...)
      // vtkInteractorStyleImage::OnMouseWheelBackward();
   }
};

vtkStandardNewMacro(myVtkInteractorStyleImage);

int main(int argc, char * argv[])
{
   // adapt path !
   if(argc < 2)
   {
      cout << "Usage: MetaIMageWriter.exe [filename]"<<endl;
      return EXIT_FAILURE;
   }

 
   // Read and display file for verification that it was written correctly

  
  vtkSmartPointer<vtkMetaImageReader> reader = vtkSmartPointer<vtkMetaImageReader>::New();
   reader->SetFileName(argv[1]);
   /*reader->SetDataExtent(1, 129, 1, 142, 1, 177);
   reader->SetDataSpacing(1,1,1);
   reader->SetDataOrigin(0.0, 0.0, 0.0);
   reader->SetDataScalarTypeToUnsignedInt();
   reader->SetFileDimensionality(3);
   reader->SetDataByteOrderToLittleEndian();
   //reader->SetNumberOfScalarComponents(2);*/
   reader->Update();

   vtkSmartPointer<vtkImageViewer2> imageViewer = vtkSmartPointer<vtkImageViewer2>::New();
   imageViewer->SetInputConnection(reader->GetOutputPort());
   imageViewer->GetRenderer()->ResetCamera();
   //imageViewer->SetSlice(0);
   imageViewer->Render();
   
   vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
   vtkSmartPointer<myVtkInteractorStyleImage> myStyle = vtkSmartPointer<myVtkInteractorStyleImage>::New();
   myStyle->SetImageViewer(imageViewer);
   imageViewer->SetupInteractor(interactor);
   interactor->SetInteractorStyle(myStyle);

   /*vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
   vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
   vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
   vtkSmartPointer<vtkInteractorStyleTrackballCamera> interactorStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
   vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
   vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
   vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
   vtkSmartPointer<vtkPiecewiseFunction> gradientOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
   vtkSmartPointer<vtkPiecewiseFunction> scalarOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
   vtkSmartPointer<vtkColorTransferFunction> color = vtkSmartPointer<vtkColorTransferFunction>::New();
   vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();

   //testing gl stuff
   //vtkOpenGLRenderWindow* glRenderWindow = vtkOpenGLRenderWindow::SafeDownCast(renderWindow);

    //glRenderWindow->SetUseOffScreenBuffers(true);

   imageData->ShallowCopy(reader->GetOutput());
   
   //renderer->SetBackground(0.1, 0.2, 0.3);

   renderWindow->AddRenderer(renderer);
   renderWindow->SetSize(500, 500);
   //renderWindow->SetMultiSamples(0);
   renderWindowInteractor->SetInteractorStyle(interactorStyle);
   renderWindowInteractor->SetRenderWindow(renderWindow);

   volumeMapper->SetBlendModeToComposite();
   volumeMapper->SetRequestedRenderModeToGPU();
   volumeMapper->SetInputData(imageData);

   volumeProperty->SetInterpolationTypeToLinear();
   gradientOpacity->AddPoint(99,0);
   gradientOpacity->AddPoint(100,1);
   //volumeProperty->SetGradientOpacity(gradientOpacity);

   scalarOpacity->AddPoint(0, -0.01);
   scalarOpacity->AddPoint(100, .00);
   scalarOpacity->AddPoint(110.01, .05);
   scalarOpacity->AddPoint(3000.0, .05);
   volumeProperty->SetScalarOpacityUnitDistance(.05);
   volumeProperty->SetScalarOpacity(scalarOpacity);


   color->AddRGBPoint(-100,0,0,0);
   color->AddRGBPoint(100.0, 0.5, 0.5, 0.5);
   volumeProperty->SetColor(color);
   volume->SetMapper(volumeMapper);
   volume->SetProperty(volumeProperty);
   renderer->AddVolume(volume);

   renderer->ResetCamera();
   renderWindow->Render();
   renderWindow->SetPosition(700,0);
   renderWindow->SetWindowName("3D Render");
*/
   interactor->Start();
 
   return EXIT_SUCCESS;
}