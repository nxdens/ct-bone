#pragma once
class dsxPose{
public:
	dsxPose();
	~dsxPose();
	 static dsxPose * New()
    {
        dsxPose * commander = new dsxPose();
        return commander;
    }
	double * getPosition();
	double * getRotation();
	double getScale();
	void translate(double translateX,double translateY,double translateZ);
	void rotate(double roll,double yaw,double pitch);
	void scale(double scaleFactor);
	dsxPose * applyPose(dsxPose newPose);
	//print function maybe?
private:
	//maybe transformation matrix?
	double mTranslateX,mTranslateY,mTranslateZ; //position values
	double mRoll, mYaw, mPitch;//rotation values
	double mScaleFactor;//scale 


}