#include "dsxXrayImage.h"

dsxXrayImage::dsxXrayImage()
{
}

dsxXrayImage::~dsxXrayImage()
{
}

void dsxXrayImage::initialize(char * imageName, char * cubeName, int startFrame)
{
    imreadmulti(imageName,imageStack);
    currentFrame = startFrame;
    currentImage = imageStack[currentFrame];
    parameters.findParams(cubeName);
}

void dsxXrayImage::toImageData( vtkImageData * im)
{
    int rows = imageStack[0].rows;
    int cols = imageStack[0].cols;
    int slices = imageStack.size();
    im->SetDimensions(rows+1,cols+1,slices+1);
    im->AllocateScalars(VTK_UNSIGNED_CHAR,3);
    unsigned char* currentPixel;
    for(int k = 0; k < slices ; k++)
    {
        for(int i = 0; i < rows; i++)
        {
            for(int j = 0; j< cols;j++)
            {
                currentPixel = static_cast<unsigned char*>(im->GetScalarPointer(i,j,k));
                *currentPixel = imageStack[k].data[j*rows+i];
                *(currentPixel+1) = imageStack[k].data[j*rows+i];
                *(currentPixel+2) = imageStack[k].data[j*rows+i];
                //cout << imageStack[k].data[j*3*rows+i*3] << endl;
            }
        }
    }
}

double dsxXrayImage::getCubeParameterAt(int i)
{
    return parameters.cubeParameters.at(i);
}
