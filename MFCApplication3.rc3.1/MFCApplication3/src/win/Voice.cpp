#include "../Voice.h"

#include "Formater.h"

#include <exception>

#include <wrl.h>

#include <xaudio2.h>


using namespace Microsoft::WRL;//ComPtr

class VoiceData
{
public:
	VoiceData(){}
	WAVEFORMATEX waveFormat;
	ComPtr<IXAudio2> XAudio2;
	IXAudio2MasteringVoice* pMasterVoice = NULL;
	IXAudio2SourceVoice* pSourceVoice = NULL;

	boost::shared_array<XAUDIO2_BUFFER> aBuffer;
	WORD numberOfBuffers;
};

Voice::Voice(long nChannels, long nSamplesPerSec, long numberOfBuffers, long bufferLength)//Formater formater, WORD numberOfBuffers, WORD bufferLength)
	: pVoiceData(new VoiceData())
{
	try{
		pVoiceData->numberOfBuffers = numberOfBuffers;
		pVoiceData->aBuffer = boost::shared_array<XAUDIO2_BUFFER>(new XAUDIO2_BUFFER[numberOfBuffers]);

		HRESULT hr(0);
		Formater formater(nChannels, nSamplesPerSec);
		formater.FormatWave(&pVoiceData->waveFormat);

		if (FAILED(hr = XAudio2Create(&pVoiceData->XAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
			throw std::exception("Can not create XAudio2 engine");
		else if (FAILED(hr = pVoiceData->XAudio2->CreateMasteringVoice(&pVoiceData->pMasterVoice))){
			throw std::exception("Can not create IXAudio2MasteringVoice interface");
		}

		if (FAILED(hr = pVoiceData->XAudio2->CreateSourceVoice(&pVoiceData->pSourceVoice, &pVoiceData->waveFormat))){
			throw std::exception("Can not create IXAudio2SourceVoice interface");
		}

		for (int i = 0; i < numberOfBuffers; i++){
			XAUDIO2_BUFFER *pBuf = &pVoiceData->aBuffer[i];
			formater.FormatWaveBuffer(pBuf, bufferLength);
			pBuf->pAudioData = new BYTE[pBuf->AudioBytes];
		}
	}
	catch (std::exception ex){
		Clean();
		throw ex;
	}
}


Voice::~Voice()
{
	Clean();
}

void Voice::Clean()
{
	if (pVoiceData->pSourceVoice != NULL)
		pVoiceData->pSourceVoice->DestroyVoice();
	if (pVoiceData->pMasterVoice != NULL)
		pVoiceData->pMasterVoice->DestroyVoice();

	pVoiceData->pSourceVoice = NULL;
	pVoiceData->pMasterVoice = NULL;

	for (int i = 0; i < pVoiceData->numberOfBuffers; i++)
		if (pVoiceData->aBuffer[i].pAudioData != NULL){
		delete pVoiceData->aBuffer[i].pAudioData;
		pVoiceData->aBuffer[i].pAudioData = NULL;
		}
}

void Voice::startVoice()
{
	HRESULT hr(0);
	if (!pVoiceData->pSourceVoice || FAILED(hr = pVoiceData->pSourceVoice->Start(0))){
		throw std::exception("Can not start source voice");
	}
}

long Voice::getNumberOfBuffers()
{
	return pVoiceData->numberOfBuffers;
}

long Voice::getNumberOfFreeBuffers()
{
	XAUDIO2_VOICE_STATE voiceState;
	pVoiceData->pSourceVoice->GetState(&voiceState);
	return pVoiceData->numberOfBuffers - voiceState.BuffersQueued;
}

long Voice::getBufferLength()
{
	return pVoiceData->aBuffer[0].PlayLength;
}

float** Voice::getBuffer(long iBuffer)
{
	return (float**)&(pVoiceData->aBuffer[iBuffer].pAudioData);
}

void Voice::submitBuffer(long iBuffer)
{
	HRESULT hr(0);
	if (!pVoiceData->pSourceVoice || FAILED(hr = pVoiceData->pSourceVoice->SubmitSourceBuffer(&pVoiceData->aBuffer[iBuffer]))){
		throw std::exception("Can not submit buffer");
	}

}

long Voice::getSamplesPerSec()
{
	return pVoiceData->waveFormat.nSamplesPerSec;//20ms (192000hz,4096 samples)
}
