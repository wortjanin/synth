#pragma once

#include <xaudio2.h>

class Formater
{
public:
	void FormatWave(WAVEFORMATEX* pWaveFormat);
	void FormatWaveBuffer(XAUDIO2_BUFFER* pWaveBuffer, UINT32 nPlayLength);

	//nChannels = 1 or 2, nSamplesPerSec = 44100, 48000, 96000, or 192000
	Formater(WORD nChannels, DWORD nSamplesPerSec);
	virtual ~Formater();

private:
	WAVEFORMATEX waveFormat;
	XAUDIO2_BUFFER waveBuffer;
};

