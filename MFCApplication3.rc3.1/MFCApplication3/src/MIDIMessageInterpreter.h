#pragma once

#include "Instrument.h"
#include "MIDIMessage.h"

class Instrument;

class MIDIMessageInterpreter
{
public:
	MIDIMessageInterpreter(int nKeyboardSize = 128);

	void SendMessageToInstrument(
		Instrument* pInstrument, 
		MIDIMessage* pMessage,
		bool isFirst, 
		bool isLast);

	virtual ~MIDIMessageInterpreter();
private:
	int nKeyboardSize;
};

