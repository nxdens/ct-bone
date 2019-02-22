#include "dsxBone.h"

dsxBone::dsxBone() 
{
	currentPose = new dsxPose();
	mCtBoneVoxels = vtkSmartPointer<vtkVolume>::New();
	mCtBoneMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
	mCtImageData = vtkSmartPointer<vtkImageData>::New();
	mCtCompositeOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	mCtBoneVoxels->SetMapper(mCtBoneMapper);
	mCtBoneMapper->SetRequestedRenderModeToGPU();
	mCtCompositeOpacity->AddPoint(99,0.0);
	mCtCompositeOpacity->AddPoint(100.0,1);
	mInternalTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
}
dsxBone::dsxBone(int totalFrames)
{
	mTotalFrames = totalFrames;
	dsxBone();
}
dsxBone::~dsxBone()
{}

void dsxBone::setBone(vtkImageData * ctImageData)
{
	mCtImageData->DeepCopy(ctImageData);
	mCtBoneMapper->SetInputData(mCtImageData);
	mCtBoneVoxels->GetProperty()->SetScalarOpacity(mCtCompositeOpacity);
	//maybe get the initial position from this?
}

void dsxBone::rotate(double roll,double yaw,double pitch)
{
	currentPose->rotate(roll,yaw,pitch);
	mCtBoneVoxels->RotateX(roll);
	mCtBoneVoxels->RotateY(yaw);
	mCtBoneVoxels->RotateZ(pitch);
	//mCtBoneVoxels->AddOrientation(roll, yaw, pitch)
}
void dsxBone::translate(double translateX,double translateY,double translateZ)
{
	mCtBoneVoxels->AddPosition(translateX,translateY,translateZ);
	currentPose->translate(translateX,translateY,translateZ);
}
void dsxBone::scale(double scaleFactor)
{
	mCtBoneVoxels->SetScale(scaleFactor);
	currentPose->scale(scaleFactor);
}
dsxPose dsxBone::recordPose()
{
	oldPose = *currentPose;
	mCtBoneVoxels->ComputeMatrix();
	mCtBoneVoxels->GetMatrix(mInternalTransformMatrix);
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
void dsxBone::saveCurrentPose()
{
	poseSequence.push_back(*currentPose);
}