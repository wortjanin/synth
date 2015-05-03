#include "DSPPackInputs.h"
#include "DSPSoloSinWave.h"

#include <complex>

DSPPackInputs::DSPPackInputs(long nChannels)
	:nChannels(nChannels)
{
}


DSPPackInputs::~DSPPackInputs()
{
}


void DSPPackInputs::process(float **inputs, float **outputs, long length)
{
}

void DSPPackInputs::processReplacing(float **inputs, float **outputs, long length)
{
	long M = length*nChannels;
	for (long n = 0; n < nChannels; n++)
		for (long m = 0, k = 0; m < M; m += nChannels, k++)
			*((*outputs) + m + n) = *((*inputs) + k + n*nChannels);
}

void DSPPackInputs::setParameter(long index, float value)
{

}

float DSPPackInputs::getParameter(long index)
{
	return 0;
}
