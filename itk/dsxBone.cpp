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

	//needs this to be able to render the data since default opacity for everything is 0
	mCtCompositeOpacity->AddPoint(99,0.0);
	//just sets everything to 1 can use multiple piecewise function points to threshold certain densities
	mCtCompositeOpacity->AddPoint(100.0,1);
	mCtBoneVoxels->GetProperty()->SetScalarOpacity(mCtCompositeOpacity);
	//mInternalTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
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
void dsxBone::setPosition(double setX, double setY, double setZ)
{
	mCtBoneVoxels->SetPosition(setX,setY,setZ);
	currentPose->setPosition(setX,setY,setZ);
}
void dsxBone::scale(double scaleFactor)
{
	mCtBoneVoxels->SetScale(scaleFactor,scaleFactor,scaleFactor);
	currentPose->scale(scaleFactor);
}
dsxPose dsxBone::recordPose()
{
	mCtBoneVoxels->ComputeMatrix();
	mCtBoneVoxels->GetMatrix(mInternalTransformMatrix);
	currentPose->saveMatrix(mInternalTransformMatrix);
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
	
	double * newPosition = newPose.getPosition();
	double * newRotation = newPose.getRotation();
	double newScale = newPose.getScale();
	mCtBoneVoxels->SetPosition(newPosition[0], newPosition[1],newPosition[2]);
	mCtBoneVoxels->SetOrientation(newRotation[0], newRotation[1],newRotation[2]);
	mCtBoneVoxels->SetScale(newScale,newScale,newScale);

	return &oldPose;
}
vtkVolume * dsxBone::getVolume()
{
	return mCtBoneVoxels;
}
void dsxBone::saveCurrentPose()
{
	poseSequence.push_back(*currentPose);
}