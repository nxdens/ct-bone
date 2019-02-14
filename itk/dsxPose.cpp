#include "dsxPose.h"

dsxPose::dsxPose()
{
	mTranslateX = 0;
	mTranslateY = 0;
	mTranslateZ = 0;
	mRoll = 0;
	mYaw = 0;
	mPitch = 0;
	mScaleFactor = 0;
}

dsxPose::~dsxPose();

void translate(double translateX,double translateY,double translateZ);
{
	this->mTranslateX = translateX;
	this->mTranslateY = translateY;
	this->mTranslateZ = translateZ;
}

void rotate(double roll,double yaw, double pitch)
{
	this->mRoll = roll;
	this->mYaw = yaw;
	this->mPitch = pitch;
}

void scale(double scaleFactor)
{
	this->mScaleFactor = scaleFactor;
}
double * getPosition()
{
	return {this->mTranslateX,this->mTranslateY, this->mTranslateZ};
}

double * getRotation()
{
	return {this->mRoll,this->mYaw, this->mPitch};
}

double getScale()
{
	return this->mScaleFactor;
}

dsxPose * applyPose(dsxPose newPose)
{
	double* tPosition = newPose.getPosition();
	double* tOrrientation = newPose.getRotation();
	double tScale = getScale();
	this->translate(tPosition[0], tPosition[1],tPosition[2])
	this->rotate(tOrrientation[0], tOrrientation[1],tOrrientation[2])
	this->scale(tScale);
	return &this;
}
