#include "MIDIMessageQueue.h"
#include <boost/thread/thread.hpp>


MIDIMessageQueue::MIDIMessageQueue(size_t capacity)
{
	queue.set_capacity(capacity);
}


void MIDIMessageQueue::enque(MIDIMessage midiMessage)
{
	boost::lock_guard<boost::mutex> lock(queueMutex);

	queue.push_back(midiMessage);
}

bool MIDIMessageQueue::deque(MIDIMessage* pMidiMessage)
{
	boost::lock_guard<boost::mutex> lock(queueMutex);

	if (queue.empty())
		return false;

	memcpy(pMidiMessage, &queue[0], sizeof(MIDIMessage));
	
	if (queue[0].getMessageType() == MIDIMessage::Close){
		queue.clear();
		return true;
	}

	queue.pop_front();
	return true;
}

void MIDIMessageQueue::reset(){
	boost::lock_guard<boost::mutex> lock(queueMutex);
	queue.clear();
}

MIDIMessageQueue::~MIDIMessageQueue()
{
}
