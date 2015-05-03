#include "Signal.h"

#include <boost/multi_array.hpp>

Signal::Signal(long nChannels, long length, boost::shared_ptr<ISignalProcessor> pDSP, boost::shared_ptr<ISignalProcessor> pDSPMergeInputs)
{
	this->nChannels = nChannels;
	this->length = length;
	dspVector.resize(2);
	dspVector[0] = pDSP;
	dspVector[1] = pDSPMergeInputs;
	
	pInputs = boost::shared_array<float *>(new float*[nChannels]);
	pOutputs = boost::shared_array<float *>(new float*[nChannels]);
	pMemory = boost::shared_array<float>(new float[2 * nChannels*length]);
	for (int n = 0; n < nChannels; n++)
		pInputs[n] = &(pMemory[n*length]);
	for (int n = 0; n < nChannels; n++)
		pOutputs[n] = &(pMemory[(n + nChannels)*length]);
}

void Signal::addDSP(boost::shared_ptr<ISignalProcessor> pSignalProcessor, long before)
{
	assert(0 <= before && before <= getDSPCount());

	dspVector.resize(dspVector.size() + 1);
	dspVector.insert(std::find(dspVector.begin(), dspVector.end(), dspVector[before + 1]), pSignalProcessor);
}

void Signal::removeDSP(long index)
{
	assert(0 <= index && index < getDSPCount());
	dspVector.erase(std::find(dspVector.begin(), dspVector.end(), dspVector[index + 1]));
}

boost::shared_ptr<ISignalProcessor> Signal::getDSP(long index)
{
	return dspVector[index + 1];
}

long Signal::getDSPCount()
{
	return dspVector.size() - 2;
}

void Signal::put(float **ppInto)
{
	float **inputs = pInputs.get();
	float **outputs = pOutputs.get();
	float **swap;
	dspVector[0]->processReplacing(inputs, outputs, length);//signal generation
	long n = 1, N = dspVector.size() - 1;
	for (; n < N; n++){
		swap = outputs;
		outputs = inputs;
		inputs = swap;
		dspVector[n]->process(inputs, outputs, length);
	}
	dspVector[n]->processReplacing(outputs, ppInto, length);//signal pack
}

boost::shared_ptr<ISignalProcessor> Signal::getSignalGenerator()
{
	return dspVector[0];
}


Signal::~Signal()
{

}
