#ifndef __DSX_BONE__
#define __DSX_BONE__
#include <vtkSmartPointer.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeMapper.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkPiecewiseFunction.h>
#include <vtkImageData.h>
#include "dsxPose.h"
#include <iostream>
#include <list>
class dsxBone
{
public:
	dsxPose * applyPose(dsxPose newPose);
	dsxPose * getPose();
	dsxPose * currentPose;
	dsxPose oldPose;
	vtkVolume * mCtBoneVoxels;
	vtkSmartVolumeMapper * mCtBoneMapper;
	vtkImageData * mCtImageData;
	std::list <dsxPose> poseSequence;
	vtkPiecewiseFunction* mCtCompositeOpacity;
	int[16] mInternalTransformMatrix;
	int mTotalFrames = 0;
	dsxBone();
	dsxBone(int totalFrames);
	~dsxBone();
	static dsxBone * New()
	{
		dsxBone *ctBone = new dsxBone();
		return ctBone;
	}
	void setBone(vtkImageData * ctImageData);
	void rotate(double roll,double yaw,double pitch);
	void translate(double translateX,double translateY,double translateZ);
	void setPosition(double setX, double setY, double setZ);
	void scale(double scaleFactor); //uniform scaling
	dsxPose recordPose();
	void restorePose();
	void saveCurrentPose();
	vtkVolume* getVolume();
	void setPosition(double translateX,double translateY,double translateZ);
};
#endif