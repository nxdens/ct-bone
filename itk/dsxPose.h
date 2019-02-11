#pragma once
class dsxPose{
public:
	int * getPosition();
	int * getRotation();
private:
	//maybe transformation matrix?
	int x,y,z; //position values
	string filename;//mhd header file
	int roll, yaw, pitch;//rotation values


}