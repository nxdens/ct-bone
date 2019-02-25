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
class dsxBone : public vtkVolume
{
public:
	dsxPose * applyPose(dsxPose newPose);
	dsxPose * getPose();
	dsxPose * currentPose;
	dsxPose oldPose;
	vtkSmartPointer<vtkVolume> mCtBoneVoxels;
	vtkSmartPointer<vtkSmartVolumeMapper> mCtBoneMapper;
	vtkSmartPointer<vtkImageData> mCtImageData;
	std::list <dsxPose> poseSequence;
	vtkSmartPointer<vtkPiecewiseFunction> mCtCompositeOpacity;
	double mInternalTransformMatrix[16];
	int mTotalFrames = 0;
	dsxBone();
	dsxBone(int totalFrames);
	~dsxBone();
	static dsxBone * New()
	{
		dsxBone * ctBone = new dsxBone();
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
};
#endif