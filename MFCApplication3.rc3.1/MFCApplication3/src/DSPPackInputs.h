#pragma once

#include "ISignalProcessor.h"

class DSPPackInputs :public ISignalProcessor
{
public:
	DSPPackInputs(long nChannels);

	/// ISignal
	virtual void process(float **inputs, float **outputs, long length);
	virtual void processReplacing(float **inputs, float **outputs, long length);

	virtual void setParameter(long index, float value);
	virtual float getParameter(long index);
	/// ~ISignal

	virtual ~DSPPackInputs();

private:
	long nChannels;
};

