#include "DSPSoloSinWave.h"

#include <assert.h>
#include <math.h>

DSPSoloSinWave::DSPSoloSinWave(float nSamplesPerSec)
	:nSamplesPerSec(nSamplesPerSec) 
{
	assert(nSamplesPerSec > 0);
	fvParameters.resize(3);
	fvParameters[0] = 0;
	fvParameters[1] = 0;
	fvParameters[2] = 0;
}


DSPSoloSinWave::~DSPSoloSinWave()
{
}

void  DSPSoloSinWave::process(float **inputs, float **outputs, long length){

}

void  DSPSoloSinWave::processReplacing(float **inputs, float **outputs, long length){

	float *out0 = *outputs;
	float *out1 = *outputs + length;

	float &N = nSamplesPerSec;
	float F = fvParameters[0];
	float A = fvParameters[1];

	std::complex<float> m = (F == 0.f) ? e : exp(j / (N / pi2 / F));

	for (long i = 0; i < length; i++){
		*out0 = A*cZeroCount.real();
		out0++;

		*out1 = A*cZeroCount.imag();
		out1++;

		cZeroCount *= m;
	}

}

// 0 - hzBaseFrequency, 1 - fAmplitude, 2 - msPhase
void  DSPSoloSinWave::setParameter(long index, float value){

	if (!index){
		float &N = nSamplesPerSec;
		cZeroCount *= exp(j*(value - fvParameters[0])/N/pi2/2.f);
	}

	fvParameters[index] = value;
}

float  DSPSoloSinWave::getParameter(long index){
	return fvParameters[index];
}
