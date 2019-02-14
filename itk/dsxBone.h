#pragma once
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeMapper.h>
#include <vtkSmartVolumeMapper.h>
#include "dsxPose.h"
class dsxBone{
public:
	dsxBone(vtkVolume * ctBoneVoxels);
	~dsxBone();
	 static dsxBone * New()
    {
        dsxBone * commander = new dsxBone();
        return commander;
    }
	void rotate(double roll,double yaw,double pitch);
	void translate(double translateX,double translateY,double translateZ);
	void scale(double c); //uniform scaling
	void recordPose();
	void restorePose();
	dsxPose * getPose();
private:
	dsxPose * currentPose;
	dsxPose oldPose;
	vtkVolume * mCtBoneVoxels;
}