#pragma once

#include "MIDIMessage.h"

#include <boost/circular_buffer.hpp>
#include <boost/thread/thread.hpp>

/// 
/// thread-safe
///
class MIDIMessageQueue
{
public:
	MIDIMessageQueue(size_t capacity);

	void enque(MIDIMessage midiMessage);
	bool deque(MIDIMessage* pMidiMessage);

	void reset();

	virtual ~MIDIMessageQueue();
private:
	boost::circular_buffer<MIDIMessage> queue;
	boost::mutex queueMutex;
};

