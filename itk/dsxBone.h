#pragma once
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeMapper.h>
#include <vtkSmartVolumeMapper.h>
#include "dsxPose.h"
class dsxBone{
public:
	dsxBone();
	~dsxBone();
	 static dsxBone * New()
    {
        dsxBone * commander = new dsxBone();
        return commander;
    }
	void rotate(double roll,double yaw,double pitch);
	void translate(double x,double y,double z);
	void scale(double c); //uniform scaling

private:
	dsxPose * pose;
	vtkVolume * dsxVoxels;
}