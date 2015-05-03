#include "Formater.h"

//#define NDEBUG  
#include <assert.h>

static WAVEFORMATEX WaveFormatInit(WORD nChannels, DWORD nSamplesPerSec){
	WORD nBytesPerSample = 4;

	WAVEFORMATEX waveFormat;

	waveFormat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;// WAVE_FORMAT_PCM;
	waveFormat.nChannels = nChannels;
	waveFormat.nSamplesPerSec = nSamplesPerSec;// 192000;//96000;
	waveFormat.wBitsPerSample = nBytesPerSample * 8;

	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nChannels * nBytesPerSample;
	waveFormat.nBlockAlign = (waveFormat.nChannels*nBytesPerSample);
	waveFormat.cbSize = 0;

	return waveFormat;
}

static XAUDIO2_BUFFER WaveBufferInit(WAVEFORMATEX* pWaveFormat){
	XAUDIO2_BUFFER waveBuffer;

	waveBuffer.pAudioData = NULL;
	waveBuffer.Flags = XAUDIO2_END_OF_STREAM;
	waveBuffer.LoopBegin = 0;// buffer.PlayBegin;
	waveBuffer.LoopLength = 0;// buffer.PlayLength;
	waveBuffer.LoopCount = 0;// 5 + 1 times -> 6 sec
	waveBuffer.pContext = NULL;
	waveBuffer.PlayBegin = 0;
	waveBuffer.PlayLength = 1;// lendth for 1 sample, multiply it for number of samples
	waveBuffer.AudioBytes = (pWaveFormat->wBitsPerSample / 8) * pWaveFormat->nChannels; // size for 1 sample, multiply it for number of samples
	
	return waveBuffer;
}

void Formater::FormatWave(WAVEFORMATEX* pWaveFormat)
{
	assert(pWaveFormat);

	memcpy(pWaveFormat, &waveFormat, sizeof(waveFormat));
}

void Formater::FormatWaveBuffer(XAUDIO2_BUFFER* pWaveBuffer, UINT32 nPlayLength)
{
	assert(pWaveBuffer && nPlayLength > 0);

	memcpy(pWaveBuffer, &waveBuffer, sizeof(waveBuffer));
	pWaveBuffer->PlayLength *= nPlayLength;
	pWaveBuffer->AudioBytes *= nPlayLength;
}

Formater::Formater(WORD nChannels, DWORD nSamplesPerSec)
{
	waveFormat = WaveFormatInit(nChannels, nSamplesPerSec);
	waveBuffer = WaveBufferInit(&waveFormat);
}


Formater::~Formater()
{
}
