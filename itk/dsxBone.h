#pragma once
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeMapper.h>
#include <vtkSmartVolumeMapper.h>
#include "dsxPose.h"
class dsxPose{
public:
	void rotate(roll,yaw,pitch);
	void translate(x,y,z);
	void scale(c);
private:
	dsxPose pose;
	vtkVolume * voxels;
}