#include "Instrument.h"
#include "Signal.h"

#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

Instrument::Instrument(boost::shared_ptr<IVoice> pVoice, boost::shared_ptr<Signal> pSignal)
	:pVoice(pVoice), pSignal(pSignal), pMIDIMessageInterpreter(new MIDIMessageInterpreter(128))
{
	pVoice->startVoice();
}

void Instrument::run(void(*pTimerHandler)(TimerHandlerInput* pInput), void* pApp, long nCycleCount)
{
	boost::thread instrumentThread(&Instrument::runThread, this, pTimerHandler, pApp, 100);
	instrumentThread.detach();
}

void Instrument::runThread(void(*TimerHandler)(TimerHandlerInput* pInput), void* pApp, long nCycleCount)
{
	doneMutex.lock();
	if (running){
		doneMutex.unlock();
		return;
	}

	using namespace boost::posix_time;

	long iBuffer = 0, nBuffer = pVoice->getNumberOfBuffers();

	long nBufferLength = pVoice->getBufferLength();
	long iCycle = 0;

	long msBufferLength = (nBufferLength * 1000) / pVoice->getSamplesPerSec();//20ms (192000hz,4096 samples)
	boost::chrono::nanoseconds ideal_ns((msBufferLength * 1000 * 1000) / 8);//ideal cycle duration is 10ms/8=1ms (192000hz,2048 samples)

	running = true;
	done = false;
	doneMutex.unlock();
	while (running){
		boost::chrono::high_resolution_clock::time_point startCycle = boost::chrono::high_resolution_clock::now();

		long freeBuffers = pVoice->getNumberOfFreeBuffers(); //nBuffer - voiceState.BuffersQueued;

		for (long b = 0; b < freeBuffers; b++, iBuffer = (iBuffer + 1) % nBuffer){
			float **ppBuffer = pVoice->getBuffer(iBuffer);
			pSignalMutex.lock();
			pSignal->put(ppBuffer);
			pSignalMutex.unlock();
			pVoice->submitBuffer(iBuffer);// >SubmitSourceBuffer(pBuffer);
		}

		boost::chrono::nanoseconds ns = boost::chrono::high_resolution_clock::now() - startCycle;
		iCycle++;
		if (iCycle / nCycleCount > 0){
			iCycle /= nCycleCount;
			TimerHandlerInput in(pApp, pVoice, ns.count() / 1000000.f);
			TimerHandler(&in);
		}
		if (ns < ideal_ns)
			boost::this_thread::sleep(microseconds((ideal_ns.count() - ns.count()) / 1000));
	}

	boost::mutex::scoped_lock lock(doneMutex);
	done = true;
	stoppedCV.notify_one();
}

void Instrument::stop(){
	boost::mutex::scoped_lock lock(doneMutex);
	if (!running)
		return;
	running = false;
	while (!done) stoppedCV.wait(lock);

	auto g = pSignal->getSignalGenerator();
	g->setParameter(0, 0);
	g->setParameter(1, 0);
	g->setParameter(2, 0);
}

void Instrument::SetWave(
	float hzBaseFrequency,
	float fAmplitude,
	float msPhase,
	bool isFirst,
	bool isLast)
{
	if (isFirst)
		pSignalMutex.lock();

	auto g = pSignal->getSignalGenerator();
	g->setParameter(0, hzBaseFrequency);
	g->setParameter(1, fAmplitude);
	g->setParameter(2, msPhase);

	if (isLast)
		pSignalMutex.unlock();
}

void Instrument::HandleMIDIMessage(MIDIMessage* pMsg, bool isFirst, bool isLast){
	pMIDIMessageInterpreter->SendMessageToInstrument(this, pMsg, isFirst, isLast);
}


Instrument::~Instrument()
{
}
