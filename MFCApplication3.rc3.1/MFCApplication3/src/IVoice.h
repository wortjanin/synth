#pragma once
class IVoice
{
public:
	virtual void startVoice() = 0;

	virtual long getNumberOfBuffers() = 0;
	virtual long getNumberOfFreeBuffers() = 0;

	virtual long getBufferLength() = 0;
	virtual float** getBuffer(long iBuffer) = 0;
	virtual void submitBuffer(long iBuffer) = 0;

	virtual long getSamplesPerSec() = 0;
};

