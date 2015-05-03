#pragma once

#include <complex>
#include <math.h>

#include "ISignalProcessor.h"

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <vector>

class Signal
{
public:
	Signal(
		long nChannels, 
		long length, 
		boost::shared_ptr<ISignalProcessor> pDSP, 
		boost::shared_ptr<ISignalProcessor> pDSPMergeInputs);

	void addDSP(boost::shared_ptr<ISignalProcessor> pSignalProcessor, long before = 0);
	void removeDSP(long index);
	boost::shared_ptr<ISignalProcessor> getDSP(long index);
	long getDSPCount();

	void put(float **ppInto);

	boost::shared_ptr<ISignalProcessor> getSignalGenerator();

	virtual ~Signal();

private:
	long nChannels;
	long length;
	std::vector<boost::shared_ptr<ISignalProcessor>> dspVector;


	boost::shared_array<float *> pInputs;
	boost::shared_array<float *> pOutputs;
	boost::shared_array<float> pMemory;

};

