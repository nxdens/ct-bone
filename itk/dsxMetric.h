#pragma once

class dsxMetric
{
public:
	dsxMetric();
	~dsxMetric();

	void   SetNumberOfFrames(int setNumberOfFrames);
	void   DisplayFullMetric(int presentTimeFrame);
	double GetScalarMetric(int presentTimeFrame);

protected:
	int    m_numberOfFrames;
	int    m_numberOfScales;
	double m_scalarMetric;
};
