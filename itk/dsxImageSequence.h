#pragma once

enum IMAGE_TYPE { DSX, DRR };
enum SCANNER { INLINE, OFFSET };

class dsxImageSequence
{
public:
	dsxImageSequence();
	~dsxImageSequence();

	void ReadImage(SCANNER scanner, IMAGE_TYPE image_type);
};

