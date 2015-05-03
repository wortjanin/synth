#pragma once

#include <boost/shared_array.hpp>

#include "IVoice.h"

class VoiceData;

class Voice : public IVoice
{
public:
	//nChannels = 1 or 2, nSamplesPerSec = 44100, 48000, 96000, or 192000
	Voice(long nChannels, long nSamplesPerSec, long numberOfBuffers, long bufferLength);
	virtual ~Voice();

	/// IVoice
	virtual void startVoice();

	virtual long getNumberOfBuffers();
	virtual long getNumberOfFreeBuffers();

	virtual long getBufferLength();
	virtual float** getBuffer(long iBuffer);
	virtual void submitBuffer(long iBuffer);

	virtual long getSamplesPerSec();
	/// ~IVoice

private:
	boost::shared_ptr<VoiceData> pVoiceData;

	void Clean();
};

