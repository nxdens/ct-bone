#include "dsxBone.h"

dsxBone::dsxBone() 
{
}

dsxBone::~dsxBone()
{}

void dsxBone::rotate(double roll,double yaw,double pitch)
{
	this->currentPose->rotate(roll,yaw,pitch);
	mCtBoneVoxels->RotateX(roll);
	mCtBoneVoxels->RotateY(yaw);
	mCtBoneVoxels->RotateZ(pitch);
	//mCtBoneVoxels->AddOrientation(roll, yaw, pitch)
}
void dsxBone::translate(double x,double y,double z)
{
	mCtBoneVoxels->AddPosition(x,y,z);
	currentPose->translate(x,y,z);
}
void dsxBone::scale(double c)
{
	mCtBoneVoxels->SetScale(c);
	currentPose->scale(c);
}
dsxPose dsxBone::recordPose()
{
	oldPose = *currentPose;
	return oldPose;
}
void dsxBone::restorePose()
{
	currentPose = currentPose->applyPose(oldPose);
}
dsxPose * dsxBone::getPose()
{
	return currentPose;
}
dsxPose * dsxBone::applyPose(dsxPose newPose)
{
	oldPose = *currentPose;
	*currentPose = newPose;
	return &oldPose;
}