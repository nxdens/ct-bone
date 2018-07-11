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
 
 
// helper class to format slice status message
class StatusMessage {
public:
   static std::string Format(int slice, int maxSlice) {
      std::stringstream tmp;
      tmp << "Slice Number  " << slice + 1 << "/" << maxSlice + 1;
      return tmp.str();
   }
};
 
 
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
      cout << "Slicer: Min = " << _MinSlice << ", Max = " << _MaxSlice << std::endl;
   }
 
   void SetStatusMapper(vtkTextMapper* statusMapper) {
      _StatusMapper = statusMapper;
   }
 
 
protected:
   void MoveSliceForward() {
      if(_Slice < _MaxSlice) {
         _Slice += 1;
         cout << "MoveSliceForward::Slice = " << _Slice << std::endl;
         _ImageViewer->SetSlice(_Slice);
         std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
         _StatusMapper->SetInput(msg.c_str());
         _ImageViewer->Render();
      }
   }
 
   void MoveSliceBackward() {
      if(_Slice > _MinSlice) {
         _Slice -= 1;
         cout << "MoveSliceBackward::Slice = " << _Slice << std::endl;
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

      //status messages showing frame number
         //text properties
      vtkSmartPointer<vtkTextProperty> sagittalTextProp = vtkSmartPointer<vtkTextProperty>::New();
      sagittalTextProp->SetFontFamilyToCourier();
      sagittalTextProp->SetFontSize(20);
      sagittalTextProp->SetVerticalJustificationToBottom();
      sagittalTextProp->SetJustificationToLeft();
         //actual text
      vtkSmartPointer<vtkTextMapper> sagittalTextMapper = vtkSmartPointer<vtkTextMapper>::New();
      std::string sagMsg = StatusMessage::Format(sagittalViewer->GetSliceMin(), sagittalViewer->GetSliceMax());
      sagittalTextMapper->SetInput(sagMsg.c_str());
      sagittalTextMapper->SetTextProperty(sagittalTextProp);
         //puts creates actor to wrap the text onto the render frame
      vtkSmartPointer<vtkActor2D> sagittalTextActor = vtkSmartPointer<vtkActor2D>::New();
      sagittalTextActor->SetMapper(sagittalTextMapper);
      sagittalTextActor->SetPosition(15,10);
      //add it to the viewer
      sagittalViewer->GetRenderer()->AddActor2D(sagittalTextActor);

   //create interactors
   vtkSmartPointer<vtkRenderWindowInteractor> sagittalInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
   vtkSmartPointer<myVtkInteractorStyleImage> mySagittalInteractor = vtkSmartPointer<myVtkInteractorStyleImage>::New();

   //condifure myinteractorstyle
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

   axialViewer->GetRenderWindow()->SetWindowName("Axial");

      //status messages showing frame number
         //text properties
      vtkSmartPointer<vtkTextProperty> axialTextProp = vtkSmartPointer<vtkTextProperty>::New();
      axialTextProp->SetFontFamilyToCourier();
      axialTextProp->SetFontSize(20);
      axialTextProp->SetVerticalJustificationToBottom();
      axialTextProp->SetJustificationToLeft();
         //actual text
      vtkSmartPointer<vtkTextMapper> axialTextMapper = vtkSmartPointer<vtkTextMapper>::New();
      std::string axMsg = StatusMessage::Format(axialViewer->GetSliceMin(), axialViewer->GetSliceMax());
      axialTextMapper->SetInput(axMsg.c_str());
      axialTextMapper->SetTextProperty(axialTextProp);
         //puts creates actor to wrap the text onto the render frame
      vtkSmartPointer<vtkActor2D> axialTextActor = vtkSmartPointer<vtkActor2D>::New();
      axialTextActor->SetMapper(axialTextMapper);
      axialTextActor->SetPosition(15,10);
      //add text to viewer
      axialViewer->GetRenderer()->AddActor2D(axialTextActor);

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
   coronalViewer->GetRenderWindow()->SetWindowName("Coronal");

      //status messages showing frame number
         //text properties
      vtkSmartPointer<vtkTextProperty> coronalTextProp = vtkSmartPointer<vtkTextProperty>::New();
      coronalTextProp->SetFontFamilyToCourier();
      coronalTextProp->SetFontSize(20);
      coronalTextProp->SetVerticalJustificationToBottom();
      coronalTextProp->SetJustificationToLeft();
         //actual text
      vtkSmartPointer<vtkTextMapper> coronalTextMapper = vtkSmartPointer<vtkTextMapper>::New();
      std::string corMsg = StatusMessage::Format(coronalViewer->GetSliceMin(), coronalViewer->GetSliceMax());
      coronalTextMapper->SetInput(corMsg.c_str());
      coronalTextMapper->SetTextProperty(coronalTextProp);
         //puts creates actor to wrap the text onto the render frame
      vtkSmartPointer<vtkActor2D> coronalTextActor = vtkSmartPointer<vtkActor2D>::New();
      coronalTextActor->SetMapper(coronalTextMapper);
      coronalTextActor->SetPosition(15,10);
      //add text to the viewer
      coronalViewer->GetRenderer()->AddActor2D(coronalTextActor);

   //create interactors
   vtkSmartPointer<vtkRenderWindowInteractor> coronalInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
   vtkSmartPointer<myVtkInteractorStyleImage> myCoronalInteractor = vtkSmartPointer<myVtkInteractorStyleImage>::New();

   //condifure myinteractorstyle
   myCoronalInteractor->SetImageViewer(coronalViewer);
   myCoronalInteractor->SetStatusMapper(coronalTextMapper);

   //setup interactor
   coronalViewer->SetupInteractor(coronalInteractor);
   coronalInteractor->SetInteractorStyle(myCoronalInteractor);
   //render
   coronalViewer->GetRenderer()->ResetCamera();
   coronalViewer->Render();
   //end coronal initialization
   sagittalViewer->GetRenderWindow()->SetWindowName("Sagittal");
   coronalInteractor->Start();


   return EXIT_SUCCESS;
}


