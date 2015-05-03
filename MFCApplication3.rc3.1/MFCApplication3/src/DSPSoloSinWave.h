#pragma once

#include "ISignalProcessor.h"

#include <complex>
#include <vector>
//#include <boost/shared_array.hpp>


/// CONSTANTS
const std::complex<float> j = std::complex<float>(0, 1);
const float pi = 2 * asin(1.F);// asin(1) = PI/2, pi = PI
const float pi2 = 2 * pi;
const std::complex<float> e = exp(j * pi / 4.f);
/// ~CONSTANTS


class DSPSoloSinWave : public ISignalProcessor
{
public:
	/// ISignal
	virtual void process(float **inputs, float **outputs, long length);
	virtual void processReplacing(float **inputs, float **outputs, long length);

	// 0 - hzBaseFrequency, 1 - fAmplitude, 2 - msPhase
	virtual void setParameter(long index, float value);
	virtual float getParameter(long index);
	/// ~ISignal


	DSPSoloSinWave(float nSamplesPerSec);
	virtual ~DSPSoloSinWave();

private:
	float nSamplesPerSec;
	std::complex<float> cZeroCount = e;

	std::vector<float> fvParameters;
};

