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

dsxPose::~dsxPose()
{}

void dsxPose::translate(double translateX,double translateY,double translateZ)
{
	this->mTranslateX += translateX;
	this->mTranslateY += translateY;
	this->mTranslateZ += translateZ;
	mPosePositions[1] += translateX;
	mPosePositions[2] += translateY;
	mPosePositions[3] += translateZ;
}
void dsxPose::setPosition(double setX, double setY, double setZ)
{
	this->mTranslateX = translateX;
	this->mTranslateY = translateY;
	this->mTranslateZ = translateZ;
	mPosePositions[1] = translateX;
	mPosePositions[2] = translateY;
	mPosePositions[3] = translateZ;
}
void dsxPose::rotate(double roll,double yaw, double pitch)
{
	this->mRoll += roll;
	this->mYaw += yaw;
	this->mPitch += pitch;
	mPoseRotations[1] += roll;
	mPoseRotations[2] += yaw;
	mPoseRotations[3] += pitch;
}

void dsxPose::scale(double scaleFactor)
{
	this->mScaleFactor = scaleFactor;
}
double * dsxPose::getPosition()
{
	return mPosePositions;
}

double * dsxPose::getRotation()
{
	return mPoseRotations;
}

double dsxPose::getScale()
{
	return this->mScaleFactor;
}

dsxPose * dsxPose::applyPose(dsxPose newPose)
{
	double* tPosition = newPose.getPosition();
	double* tOrrientation = newPose.getRotation();
	double tScale = getScale();
	this->translate(tPosition[0], tPosition[1],tPosition[2]);
	this->rotate(tOrrientation[0], tOrrientation[1],tOrrientation[2]);
	this->scale(tScale);
	return this;
}

void dsxPose::saveMatrix(int * transformMatrix)
{
	for(int i = 0; i <16; i++)
	{
		mInternalTransformMatrix[i] = transformMatrix[i];
	}
}