#ifndef __DSX_BONE__
#define __DSX_BONE__

#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeMapper.h>
#include <vtkSmartVolumeMapper.h>
#include "dsxPose.h"
#include <iostream>
class dsxBone
{
public:
	dsxBone();
	~dsxBone();
	static dsxBone * New()
	{
		dsxBone *ctBone = new dsxBone();
		return ctBone;
	}
	void rotate(double roll,double yaw,double pitch);
	void translate(double translateX,double translateY,double translateZ);
	void scale(double c); //uniform scaling
	dsxPose recordPose();
	void restorePose();
	dsxPose * applyPose(dsxPose newPose);
	dsxPose * getPose();
	dsxPose * currentPose;
	dsxPose oldPose;
	vtkVolume * mCtBoneVoxels;
	//std::list <dsxPose> poseSequence;
};
#endif