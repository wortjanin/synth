#pragma once
class ISignalProcessor
{
public:
	virtual void process(float **inputs, float **outputs, long length) = 0;
	virtual void processReplacing(float **inputs, float **outputs, long length) = 0;

	// 0 - hzBaseFrequency, 1 - fAmplitude, 2 - msPhase
	virtual void setParameter(long index, float value) = 0;
	virtual float getParameter(long index) = 0;
};

