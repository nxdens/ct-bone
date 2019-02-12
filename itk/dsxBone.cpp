#include "dsxBone.h"

dsxBone::dsxBone();

dsxBone::~dsxBone();

void rotate(double roll,double yaw,double pitch)
{
	pose->rotate(roll,yaw,pitch);
	dsxVoxels->Roll(roll);
	dsxVoxels->Yaw(yaw);
	dsxVoxels->Pitch(pitch);
	//dsxVoxels->AddOrientation(roll, yaw, pitch)
}
void translate(double x,double y,double z)
{
	dsxVoxels->AddPosition({x,y,z});
	pose->translate(x,y,z);

}
void scale(double c)
{
	dsxVoxels->SetScale(c);
	pose->scale(c)
}
