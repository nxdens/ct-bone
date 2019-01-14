#pragma once
#include "dsxGUI.h"
#include "dsxMetric.h"
#include "dsxImageSequence.h"

class dsxOptimizer
{
public:
	dsxOptimizer();
	~dsxOptimizer();

	void Initialize(dsxGUI * setGUI, int argc, char *argv[]);
	void IterateOpitmizer();
	bool MetricAboveThreshold();
	void UpdatePoseFromTrackball();

protected:
	dsxGUI           * gui;
	dsxMetric        * metric;


	dsxImageSequence   DSXimageSequenceInline;
	dsxImageSequence   DSXimageSequenceOffset;

	dsxImageSequence   DRRimageSequenceInline;
	dsxImageSequence   DRRimageSequenceOffset;



};

