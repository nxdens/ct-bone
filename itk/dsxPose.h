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
	void translate(double x,double y,double z);
	void rotate(double roll,double yaw,double pitch);
	void scale(double c);
private:
	//maybe transformation matrix?
	double x,y,z; //position values
	string filename;//mhd header file
	double roll, yaw, pitch;//rotation values
	double c;//scale 


}