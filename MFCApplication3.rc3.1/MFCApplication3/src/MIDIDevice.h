#pragma once

#include "MIDIMessageQueue.h"
#include "MIDIMessage.h";

#include "Instrument.h"

#include <boost/shared_ptr.hpp>

class MIDIDevice
{
public:
	/// productName could be "USB MIDI Interface"
	MIDIDevice(
		const char* productName, 
		int dwBufferCount = 4, 
		int dwBufferLength = 1024,
		int midiMessageQueueCapacity = 1024);

	void run(
		boost::shared_ptr<Instrument> pInstrument);

	void stop();

	void enque(MIDIMessage midiMessage);

	virtual ~MIDIDevice();

private:
	void MidiInHandler();
	
	void start(
		const char* productName, 
		int dwBufferCount,
		int dwBufferLength);

	char* productName = 0;
	int dwBufferCount;
	int dwBufferLength;

	boost::mutex doneMutex;
	boost::condition_variable stoppedCV;
	volatile bool running = false;
	volatile bool done = false;

	boost::shared_ptr<MIDIMessageQueue> pMIDIMessageQueue;
	boost::shared_ptr<Instrument> pInstrument = NULL;
};

