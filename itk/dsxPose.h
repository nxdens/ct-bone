#ifndef _DSX_POSE_
#define _DSX_POSE_
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
	void setPosition(double setX, double setY, double setZ);
	void rotate(double roll,double yaw,double pitch);
	void scale(double scaleFactor);
	void recordPose();
	void restorePose();
	dsxPose * getPose();
	dsxPose * applyPose(dsxPose newPose);
	void saveMatrix();//deep copy
	//print function maybe?
private:
	//maybe transformation matrix?
	double mTranslateX,mTranslateY,mTranslateZ; //position values
	double mRoll, mYaw, mPitch;//rotation values
	double mScaleFactor;//scale 
	double mPoseRotations[3];
	double mPosePositions[3]; 
	int[16] mInternalTransformMatrix;

};
#endif