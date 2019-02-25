#include "dsxGrabBufferActor.h"

dsxGrabBufferActor::dsxGrabBufferActor()
{
}

dsxGrabBufferActor::~dsxGrabBufferActor()
{
}

//this whole class needs cleaning up
void dsxGrabBufferActor::SetWindow(vtkRenderWindow* ctWindow, vtkImageViewer2* xrayViewer, vtkImageData* xrayData, vtkRenderer* offset,std::vector<double> differences,dsxBone * tDsxBone)
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

    tDsxBone->rotate(1,0,0);
    //tDsxBone->recordPose();
    mActorDsxBone = tDsxBone;
    setXray();
}

void dsxGrabBufferActor::setImageData(vtkImageData * im)// eventually switch this to window to image filter instead of this junk
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
            if(ctPixelDataMatrix[(j*(width+1)+i)*3] != 0/*NULL*/)
            {
                char intensity = ctPixelDataMatrix[(j*(width+1)+i)*3]*2;
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
void dsxGrabBufferActor::resetPose()
{
    mActorDsxBone->restorePose();
}
void dsxGrabBufferActor::startTimer()
{
    this->Interactor->CreateRepeatingTimer(40);
}

void dsxGrabBufferActor::stopTimer()
{
    this->Interactor->DestroyTimer();
}

void dsxGrabBufferActor::getMetric(unsigned char * matrx)//this blocks everthing but mostly because of the imageviewer2 just get rid of the visualization to fix that
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

void dsxGrabBufferActor::visualizeBuffer()
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

void dsxGrabBufferActor::captureBuffer()
{
    ctPixelDataMatrix = ctRenderWindow->GetPixelData(0,0,height,width,true);
    //superimposes and generates a metric
    visualizeBuffer();
}

void dsxGrabBufferActor::setXray()
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

void dsxGrabBufferActor::OnKeyPress()
{
    //get the key pressed from the interactor
    //the interactor is some sort of event sniffer that looks for specific events and the style changes only what type of handler is used for each event
    std::string key = this->Interactor->GetKeySym();
    //cout << "Pressed: "<< key << endl;
    if(key == "c")
    {
        //this gets the frame buffer from the render window and passes it to opencv to generate a metric
        captureBuffer();
        stopTimer();
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

