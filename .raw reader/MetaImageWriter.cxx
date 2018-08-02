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
#include <vtkUnsignedCharArray.h>
#include <vtkPointData.h>
#include <vtkActor2D.h>
#include <vtkPNGReader.h>
#include <vtkDataSetMapper.h>
#include <vtkImageBlend.h>
 
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
      //cout <<_Slice<<endl;
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

class grabBuffer : public vtkInteractorStyleTrackballCamera
{
   public:
      static grabBuffer * New();
      vtkTypeMacro(grabBuffer, vtkInteractorStyleTrackballCamera);
      unsigned char* matrix;
      vtkSmartPointer<vtkImageBlend> blend;
      vtkSmartPointer<vtkImageViewer2> imageViewer;
      vtkSmartPointer<vtkPNGReader> background;
   protected:
      vtkRenderWindow * _renderWindow;
      int height;
      int width;
      const char rgb = 3;

   public:
      void SetWindow(vtkRenderWindow* window)
      {
         _renderWindow = window;
         int* dim = window->GetSize();
         height = dim[0]-1;
         width = dim[1]-1;
         cout <<"height: " << height+1 <<"\t width = " << width+1 << endl;
         matrix = new unsigned char[dim[0]*dim[1]*3+1];
         //set background image
         background = vtkSmartPointer<vtkPNGReader>::New();
         background->SetFileName("background.png");



         // Display the result
         vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

         imageViewer = vtkSmartPointer<vtkImageViewer2>::New();
         imageViewer->SetInputConnection(background->GetOutputPort());
         imageViewer->SetupInteractor(renderWindowInteractor);
         imageViewer->GetRenderer()->ResetCamera();

         renderWindowInteractor->Initialize();
         renderWindowInteractor->Start();
      }
      void setImageData(vtkImageData * im)
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
                  pixel[1] = 0;
                  pixel[2] = 0;
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
      }
      void visualizeBuffer()
      {
         vtkSmartPointer<vtkImageData> im = vtkSmartPointer<vtkImageData>::New();
         setImageData(im);

         blend = vtkSmartPointer<vtkImageBlend>::New();
         blend->AddInputData(im);
         blend->AddInputConnection(background->GetOutputPort());
         blend->SetOpacity(0,.5);
         blend->SetOpacity(1,.5);

         imageViewer->SetInputConnection(blend->GetOutputPort());
         imageViewer->GetRenderer()->ResetCamera();
         imageViewer->Render();
         
      }
   protected:
      void captureBuffer()
      {
         matrix = _renderWindow->GetPixelData(0,0,height,width,true);
         /*for(int i = 0 ; i<=height;i++)
         {
            for(int j = 0; j<=width;j++)
            {
               if(matrix[(i*(width+1)+j)*3] != NULL)
               {
                  cout << matrix[(i*(width+1)+j)*3] << " ";
               }
            }
         }*/
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

int main(int argc, char * argv[])
{
   // adapt path !
   if(argc < 2)
   {
      cout << "Usage: MetaIMageWriter.exe [filename]"<<endl;
      return EXIT_FAILURE;
   }

   /*
   Problems:
      1. I want to use the image reader so we dont need the header file since most of the information is in the filename
      2. with meta reader all the values are either white or black no inbetween
      3. why are there 4 images none of the parameters in the header file change that much except spacing changes the scale of the images
      4. if we have to use meta reader then I would need to generate the header file for each of the images
   */
   // Read and display file for verification that it was written correctly

  
  vtkSmartPointer<vtkMetaImageReader> reader = vtkSmartPointer<vtkMetaImageReader>::New();
   reader->SetFileName(argv[1]);
   //reader->SetNumberOfScalarComponents(2);
   reader->Update();

   vtkSmartPointer<vtkImageViewer2> imageViewer = vtkSmartPointer<vtkImageViewer2>::New();
   imageViewer->SetInputConnection(reader->GetOutputPort());
   imageViewer->GetRenderer()->ResetCamera();
   //imageViewer->SetSlice(1);
   imageViewer->Render();
   imageViewer->GetRenderer()->SetBackground(.2,.2,0);
   imageViewer->GetRenderer()->ResetCamera();

   vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
   vtkSmartPointer<myVtkInteractorStyleImage> myStyle = vtkSmartPointer<myVtkInteractorStyleImage>::New();
   myStyle->SetImageViewer(imageViewer);
   imageViewer->SetupInteractor(interactor);
   interactor->SetInteractorStyle(myStyle);

   vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
   vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
   vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
   vtkSmartPointer<grabBuffer> interactorStyle = vtkSmartPointer<grabBuffer>::New();
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

   volumeMapper->SetBlendModeToAdditive();
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
   volumeProperty->SetScalarOpacityUnitDistance(1);
   volumeProperty->SetScalarOpacity(scalarOpacity);

   volume->SetMapper(volumeMapper);
   volume->SetProperty(volumeProperty);
   renderer->AddVolume(volume);

   renderer->ResetCamera();
   renderWindow->Render();
   interactorStyle->SetWindow(renderWindow);
   renderWindow->SetPosition(700,0);
   renderWindow->SetWindowName("3D Render");

   interactor->Start();
 
   return EXIT_SUCCESS;
}