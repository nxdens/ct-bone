#include "dsxPose.h"

dsxPose::dsxPose();

dsxPose::~dsxPose();

void translate(double x,double y,double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void rotate(double roll,double yaw, double pitch)
{
	this->roll = roll;
	this->yaw = yaw;
	this->pitch = pitch;
}

void scale(double c)
{
	this->c = c;
}
int * getPosition()
{
	return {this->x,this->y, this->z};
}

int * getRotate()
{
	return {this->roll,this->yaw, this->pitch};
}

int getScale()
{
	return this->c;
}
