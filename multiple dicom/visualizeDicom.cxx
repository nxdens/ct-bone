//
// This example demonstrates how to read a series of dicom images
// and how to scroll with the mousewheel or the up/down keys
// through all slices
//
//modified 7/11/18 by Linghai Wang do display multiple views of dicom ct scans in multiple windows
// some standard vtk headers
#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
// headers needed for this example
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkInteractorStyleImage.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
// needed to easily convert int to std::string
#include <sstream>
//needed for the render 
#include <vtkImageData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>

#include <vtkOpenGLRenderWindow.h>

// helper class to format slice status message
class StatusMessage {
public:
   static std::string Format(int slice, int maxSlice) {
      std::stringstream tmp;
      tmp << "Slice Number  " << slice + 1 << "/" << maxSlice + 1;
      return tmp.str();
   }
};
class Setup{
public:
   static void setupView(vtkImageViewer2* viewer, vtkTextProperty * textProp, vtkTextMapper * textMapper, vtkActor2D * actor){
      setupText(textProp);
      std::string msg = StatusMessage::Format(viewer->GetSliceMin(), viewer->GetSliceMax());
      textMapper->SetInput(msg.c_str());
      textMapper->SetTextProperty(textProp);

      actor->SetMapper(textMapper);
      actor->SetPosition(15,10);

      viewer->GetRenderer()->AddActor2D(actor);
   }
protected:
    static void setupText(vtkTextProperty * textProp)
   { 
      textProp->SetFontFamilyToCourier();
      textProp->SetFontSize(20);
      textProp->SetVerticalJustificationToBottom();
      textProp->SetJustificationToLeft();
   }
   
};


class grabBuffer : public vtkInteractorStyleTrackballCamera
{
   public:
      static grabBuffer * New();
      vtkTypeMacro(grabBuffer, vtkInteractorStyleTrackballCamera);
      unsigned char* matrix;

   protected:
      vtkRenderWindow * _renderWindow;
      int height;
      int width;
      const char rgba = 4;

   public:
      void SetWindow(vtkRenderWindow* window)
      {
         _renderWindow = window;
         int* dim = window->GetSize();
         height = dim[0]-1;
         width = dim[1]-1;
         cout <<"height: " << height+1 <<"\t width = " << width+1 << endl;
      }
      
   protected:
      void captureBuffer(unsigned char * mat)
      {
         //_renderWindow->Render();
         mat = _renderWindow->GetPixelData(0,0,height,width,true);
         int p = 0;
         /*for(int i =0; i<(1+height)*(1+width);i+=3)//its greyscale so we only need one of the rgb values
         {
            if(mat[i] != NULL)
            {
               cout << (int)mat[i];
               p++;
               if((i+1)%3 == 0)
               {
                  cout<<endl;
               }
            }
            
         }
         cout<<endl<<p;*/
      }

      virtual void OnKeyPress()
      {
         vtkRenderWindowInteractor * rwi = this->Interactor;

         std::string key = rwi->GetKeySym();
         cout << "Pressed: "<< key << endl;
         if(key == "c")
         {
            captureBuffer(matrix);
         }
      }
};

vtkStandardNewMacro(grabBuffer);
 
// Define own interaction style
class myVtkInteractorStyleImage : public vtkInteractorStyleImage
{
public:
   static myVtkInteractorStyleImage* New();
   vtkTypeMacro(myVtkInteractorStyleImage, vtkInteractorStyleImage);
 
protected:
   vtkImageViewer2* _ImageViewer;
   vtkTextMapper* _StatusMapper;
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
 
   void SetStatusMapper(vtkTextMapper* statusMapper) {
      _StatusMapper = statusMapper;
   }
 
 
protected:
   void MoveSliceForward() {
      if(_Slice < _MaxSlice) {
         _Slice += 1;
         //cout << "MoveSliceForward::Slice = " << _Slice << std::endl;
         _ImageViewer->SetSlice(_Slice);
         std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
         _StatusMapper->SetInput(msg.c_str());
         _ImageViewer->Render();         
      }
   }
 
   void MoveSliceBackward() {
      if(_Slice > _MinSlice) {
         _Slice -= 1;
         //cout << "MoveSliceBackward::Slice = " << _Slice << std::endl;
         _ImageViewer->SetSlice(_Slice);
         std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
         _StatusMapper->SetInput(msg.c_str());
         _ImageViewer->Render();
      }
   }
 
 
   virtual void OnKeyDown() {
      std::string key = this->GetInteractor()->GetKeySym();
      if(key.compare("Up") == 0) {
         //cout << "Up arrow key was pressed." << endl;
         MoveSliceForward();
      }
      else if(key.compare("Down") == 0) {
         //cout << "Down arrow key was pressed." << endl;
         MoveSliceBackward()
;      }
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


int main(int argc, char* argv[])
{
   // Verify input arguments
   if ( argc != 2 )
   {
      std::cout << "Usage: " << argv[0]
      << " FolderName" << std::endl;
      return EXIT_FAILURE;
   }
 
   std::string folder = argv[1];
   //std::string folder = "C:\\VTK\\vtkdata-5.8.0\\Data\\DicomTestImages";
 
   // Read all the DICOM files in the specified directory.
   vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
   reader->SetDirectoryName(folder.c_str());
   reader->Update();


   // sets up each of the view ports
      //sagittal view initialization
   vtkSmartPointer<vtkImageViewer2> sagittalViewer = vtkSmartPointer<vtkImageViewer2>::New();
   sagittalViewer->SetInputConnection(reader->GetOutputPort());
   sagittalViewer->GetRenderWindow()->SetWindowName("Sagittal");

   vtkSmartPointer<vtkTextProperty> sagittalTextProp = vtkSmartPointer<vtkTextProperty>::New();
   vtkSmartPointer<vtkTextMapper> sagittalTextMapper = vtkSmartPointer<vtkTextMapper>::New();
   vtkSmartPointer<vtkActor2D> sagittalTextActor = vtkSmartPointer<vtkActor2D>::New();

   Setup::setupView(sagittalViewer, sagittalTextProp, sagittalTextMapper, sagittalTextActor);


   //create interactors
   vtkSmartPointer<vtkRenderWindowInteractor> sagittalInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
   vtkSmartPointer<myVtkInteractorStyleImage> mySagittalInteractor = vtkSmartPointer<myVtkInteractorStyleImage>::New();

   //configure myinteractorstyle
   mySagittalInteractor->SetImageViewer(sagittalViewer);
   mySagittalInteractor->SetStatusMapper(sagittalTextMapper);

   //setup interactor
   sagittalViewer->SetupInteractor(sagittalInteractor);
   sagittalInteractor->SetInteractorStyle(mySagittalInteractor);
   //render
   sagittalViewer->GetRenderer()->ResetCamera();
   sagittalViewer->Render();



      // end sagittal view initialization

      //axial view initialization
   vtkSmartPointer<vtkImageViewer2> axialViewer = vtkSmartPointer<vtkImageViewer2>::New();
   axialViewer->SetInputConnection(reader->GetOutputPort());
      //sets how to look at the slices by fixing the X and the Z axes
   axialViewer-> SetSliceOrientationToXZ(); 
   vtkSmartPointer<vtkTextProperty> axialTextProp = vtkSmartPointer<vtkTextProperty>::New();
   vtkSmartPointer<vtkTextMapper> axialTextMapper = vtkSmartPointer<vtkTextMapper>::New();
   vtkSmartPointer<vtkActor2D> axialTextActor = vtkSmartPointer<vtkActor2D>::New();

   Setup::setupView(axialViewer, axialTextProp, axialTextMapper, axialTextActor);

   //create interactors
   vtkSmartPointer<vtkRenderWindowInteractor> axialInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
   vtkSmartPointer<myVtkInteractorStyleImage> myAxialInteractor = vtkSmartPointer<myVtkInteractorStyleImage>::New();

   //condifure myinteractorstyle
   myAxialInteractor->SetImageViewer(axialViewer);
   myAxialInteractor->SetStatusMapper(axialTextMapper);

   //setup interactor
   axialViewer->SetupInteractor(axialInteractor);
   axialInteractor->SetInteractorStyle(myAxialInteractor);
   //render
   axialViewer->GetRenderer()->ResetCamera();
   axialViewer->Render();
      //end axial viewer initialization


      //coronal view initialization
   vtkSmartPointer<vtkImageViewer2> coronalViewer = vtkSmartPointer<vtkImageViewer2>::New();
   coronalViewer->SetInputConnection(reader->GetOutputPort());
   coronalViewer-> SetSliceOrientationToYZ(); 
   vtkSmartPointer<vtkTextProperty> coronalTextProp = vtkSmartPointer<vtkTextProperty>::New();
   vtkSmartPointer<vtkTextMapper> coronalTextMapper = vtkSmartPointer<vtkTextMapper>::New();
   vtkSmartPointer<vtkActor2D> coronalTextActor = vtkSmartPointer<vtkActor2D>::New();

   Setup::setupView(coronalViewer, coronalTextProp, coronalTextMapper, coronalTextActor);


   //create interactors
   vtkSmartPointer<vtkRenderWindowInteractor> coronalInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
   vtkSmartPointer<myVtkInteractorStyleImage> myCoronalInteractor = vtkSmartPointer<myVtkInteractorStyleImage>::New();

   //configure myinteractorstyle
   myCoronalInteractor->SetImageViewer(coronalViewer);
   myCoronalInteractor->SetStatusMapper(coronalTextMapper);

   //setup interactor
   coronalViewer->SetupInteractor(coronalInteractor);
   coronalInteractor->SetInteractorStyle(myCoronalInteractor);

   //render
   coronalViewer->GetRenderer()->ResetCamera();
   coronalViewer->Render();

   //end coronal initialization

   //configuring window properties
   sagittalViewer->GetRenderWindow()->SetWindowName("Sagittal");
   coronalViewer->GetRenderWindow()->SetWindowName("Coronal");
   axialViewer->GetRenderWindow()->SetWindowName("Axial");

   //window size
   sagittalViewer->GetRenderWindow()->SetSize(400,500);
   coronalViewer->GetRenderWindow()->SetSize(300,245);
   axialViewer->GetRenderWindow()->SetSize(300,225);

   //organize windows
   coronalViewer->GetRenderWindow()->SetPosition(400,0);
   axialViewer->GetRenderWindow()->SetPosition(400,275);
   sagittalViewer->Render();
   unsigned char* mat = sagittalViewer->GetRenderWindow()->GetPixelData(0,0,399,499,false);

   //setup for 3d render 
   //needs to be cleaned up
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
   //volumeProperty->SetColor(color);
   volume->SetMapper(volumeMapper);
   volume->SetProperty(volumeProperty);
   renderer->AddVolume(volume);

   renderer->ResetCamera();
   renderWindow->Render();
   renderWindow->SetPosition(700,0);
   renderWindow->SetWindowName("3D Render");
   //GetRGBACharPixelvalue is finnicky idk what conditions need to be met for this to work
      /*1 main issue (7/24) :
         1. Still cant get framebuffer using getpixeldataq
         resolved 7/25 there was never a problem the array is just filled with a lot of null values so you just need to know how many data points there are
      */
   int * dim = renderWindow->GetSize();
   unsigned char* pixels = renderWindow->GetPixelData(0,0,dim[0]-1,dim[1]-1,true);

   interactorStyle->SetWindow(renderWindow);

   unsigned char * ah = axialViewer->GetRenderWindow()->GetRGBACharPixelData(0,0,40,40,true);
   cout << sizeof(ah)/sizeof(*ah) << endl;
   //unsigned int buffer = renderWindow->GetFrontBuffer();

   coronalInteractor->Start();



   return EXIT_SUCCESS;
}


