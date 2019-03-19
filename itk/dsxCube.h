#ifndef __DSX_CUBE__
#define __DSX_CUBE__

#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>

// This gets the parameters from the document with the calibration cube optimization information
//  should be used to get the roll pitch and yaw of each camera then use that to get
//  the relative position of the offset camera

class dsxCube
{
public:
    std::vector<double> cubeParameters;  //s2d,x,y,z,roll,pitch,yaw,ppx,ppy,error^2

    dsxCube();
    ~dsxCube();

    void ParseCubeParamsFromFile(std::string filename);
};

#endif
