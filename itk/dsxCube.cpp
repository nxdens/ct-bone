#include "dsxCube.h"

dsxCube::dsxCube()
{
}

dsxCube::~dsxCube()
{
}

void dsxCube::ParseCubeParamsFromFile(std::string filename)
{
    std::ifstream cubeFile;
    cubeFile.open(filename);
    std::string currentString;
    std::string previousString;
    std::string S2D;
    //parses file to get the parameters for all the values
    
    //finds the start of the parameters
    while(getline(cubeFile,currentString))
    {
        if(currentString.find("S2D") != std::string::npos)
        {
            getline(cubeFile,currentString);
            S2D = currentString.substr(0,currentString.find("	"));
            break;
        }
    }
    //skips until it is the last line of parameters
    while(getline(cubeFile,currentString) && currentString.find(S2D) != std::string::npos)
    {
        previousString = currentString;
    }
    
    std::string::size_type previousString_pos = 0, pos = 0;
    //grabs the last row of data
    while((pos = previousString.find("	", pos)) != std::string::npos)
    {
        std::string substring(previousString.substr(previousString_pos, pos-previousString_pos) );
        cubeParameters.push_back(stod(substring));
        previousString_pos = ++pos;
    }
    cubeFile.close();
}
