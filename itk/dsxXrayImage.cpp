#include "dsxXrayImage.h"

dsxXrayImage::dsxXrayImage()
{
}

dsxXrayImage::~dsxXrayImage()
{
}

// Read the DSX image sequence and corresponding Cube file
void dsxXrayImage::Initialize(std::string imageName, std::string cubeName, int startFrame)
{
    //Use OpenCV command to read movie into 3D array m_imageStack
    imreadmulti(imageName, m_imageStack);
    m_currentFrame = startFrame;
//    currentImage = m_imageStack[m_currentFrame];
    m_cube.ParseCubeParamsFromFile(cubeName);
    
    m_vtkImageData3D = vtkSmartPointer<vtkImageData>::New();
    this->TranferImageStackToVtkImageData3D(m_vtkImageData3D);
}

int dsxXrayImage::GetNumberOfFrames(void)
{
    return( m_imageStack.size() );
}

//Read from the vector by index number 0-6
double dsxXrayImage::GetCubeParameter(int index)
{
    return m_cube.cubeParameters.at(index);
}

//Read by variable name
double dsxXrayImage::GetCubeParameterS2d(void)
{
    return m_cube.cubeParameters.at(0);
}
 
double dsxXrayImage::GetCubeParameterX(void)
{
    return m_cube.cubeParameters.at(1);
}

double dsxXrayImage::GetCubeParameterY(void)
{
    return m_cube.cubeParameters.at(2);
}

double dsxXrayImage::GetCubeParameterZ(void)
{
    return m_cube.cubeParameters.at(3);
}

double dsxXrayImage::GetCubeParameterRoll(void)
{
    return m_cube.cubeParameters.at(4);
}

double dsxXrayImage::GetCubeParameterPitch(void)
{
    return m_cube.cubeParameters.at(5);
}
double dsxXrayImage::GetCubeParameterYaw(void)
{
    return m_cube.cubeParameters.at(6);
}

vtkImageData * dsxXrayImage::GetVtkImageData(void)
{
    return(m_vtkImageData3D);
}

//Return a 2D VTK image slice from the 3D data set (in which each slice is a time frame)  NOT USED... ABANDON?
vtkSmartPointer<vtkImageSlice> dsxXrayImage::GetVtkImageSlice(int frameNumber)
{
    vtkSmartPointer<vtkImageData> imageData2D = vtkSmartPointer<vtkImageData>::New();

    vtkSmartPointer<vtkImageSliceMapper> imageSliceMapper = vtkSmartPointer<vtkImageSliceMapper>::New();
    //Default orientation is Z
    imageSliceMapper->SetSliceNumber(frameNumber);
    imageSliceMapper->SetInputData(m_vtkImageData3D);
    vtkSmartPointer<vtkImageSlice> imageSlice = vtkSmartPointer<vtkImageSlice>::New();
    imageSlice->SetMapper(imageSliceMapper);
    
    return(imageSlice);
}

// Copy single frame from m_imageStack into 2D VTK vtkImageData.  frameNumber starts at 0 !!!!!
void dsxXrayImage::GetVtkImageData2D(vtkSmartPointer<vtkImageData>imageData2D, int frameNumber)
{
    int rows   = m_imageStack[0].rows;
    int cols   = m_imageStack[0].cols;
    int slices = m_imageStack.size();
    assert(frameNumber>=0 && frameNumber<slices);
    imageData2D->SetDimensions(rows,cols,1);
    imageData2D->AllocateScalars(VTK_UNSIGNED_CHAR,3);
    unsigned char * currentPixel;
    int k = frameNumber;
    {
        for(int i = 0; i < rows; i++)
        {
            for(int j = 0; j < cols;j++)
            {
                //Find the pixel location in the VTK image
                //Flip in the y direction to make right-side-up (j is actually the y direction)
                currentPixel = static_cast<unsigned char*>(imageData2D->GetScalarPointer(i,cols-j-1,0));
                //Transfer the pixel to the R, G, and B, channels
                *currentPixel = m_imageStack[k].data[j*rows+i];
                *(currentPixel+1) = m_imageStack[k].data[j*rows+i];
                *(currentPixel+2) = m_imageStack[k].data[j*rows+i];
                //cout << m_imageStack[k].data[j*3*rows+i*3] << endl;
            }
        }
    }
}

// PRIVATE FUNCTIONS

// Copy movie from m_imageStack into 3D VTK vtkImageData
void dsxXrayImage::TranferImageStackToVtkImageData3D(vtkImageData * imageData3D)
{
    int rows = m_imageStack[0].rows;
    int cols = m_imageStack[0].cols;
    int slices = m_imageStack.size();
    imageData3D->SetDimensions(rows,cols,slices);
    imageData3D->AllocateScalars(VTK_UNSIGNED_CHAR,3);
    unsigned char * currentPixel;
    for(int k = 0; k < slices ; k++)
    {
        for(int i = 0; i < rows; i++)
        {
            for(int j = 0; j< cols;j++)
            {
                //Find the voxel location in the VTK image
                //Flip in the y direction to make right-side-up (j is actually the y direction)
                currentPixel = static_cast<unsigned char*>(imageData3D->GetScalarPointer(i,cols-j-1,k));
                //Transfer the voxel to the R, G, and B, channels
                *currentPixel = m_imageStack[k].data[j*rows+i];
                *(currentPixel+1) = m_imageStack[k].data[j*rows+i];
                *(currentPixel+2) = m_imageStack[k].data[j*rows+i];
                //cout << m_imageStack[k].data[j*3*rows+i*3] << endl;
            }
        }
    }
}

