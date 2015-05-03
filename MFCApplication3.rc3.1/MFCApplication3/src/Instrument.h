#pragma once

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include "IVoice.h"
#include "Signal.h"
#include "MIDIMessage.h"
#include "MIDIMessageInterpreter.h"

class MIDIMessageInterpreter;

class Instrument
{
public:
	class TimerHandlerInput{
	public:
		void* pApp;
		boost::shared_ptr<IVoice> pVoice;
		float msCycleDuration;
		TimerHandlerInput(void* pApp, boost::shared_ptr<IVoice> pVoice, float msCycleDuration)
			:pApp(pApp), pVoice(pVoice), msCycleDuration(msCycleDuration)
		{

		}
	};

public:
	void run(void(*TimerHandler)(TimerHandlerInput* pInput), void* pApp, long nCycleCount);
	void stop();

	void HandleMIDIMessage(MIDIMessage* pMsg, bool isFirst = true, bool isLast = true);

	void SetWave(
		float hzBaseFrequency,
		float fAmplitude,
		float msPhase = 0,
		bool isFirst = true,
		bool isLast = true);

	Instrument(boost::shared_ptr<IVoice> pVoice, boost::shared_ptr<Signal> pSignal);
	virtual ~Instrument();

private:
	void runThread(void(*TimerHandler)(TimerHandlerInput* pInput), void* pApp, long nCycleCount);

	boost::shared_ptr<IVoice> pVoice;
	boost::shared_ptr<Signal> pSignal;
	boost::shared_ptr<MIDIMessageInterpreter> pMIDIMessageInterpreter;
	boost::mutex pSignalMutex;

	boost::mutex doneMutex;
	boost::condition_variable stoppedCV;
	volatile bool running = false;
	volatile bool done = false;
};

