#include "MIDIMessageInterpreter.h"


static float* iniKeyFrequencyMap(int type, int firstKey, float baseFrequency);
const int tClean = 0, tPifagor = 1, tConstTemper = 2;
const int firstKey = 21;
static float aftertouchMax = 127.f;
static float *pKeyFrequencyMap = iniKeyFrequencyMap(tClean, firstKey, 440);

static float* iniKeyFrequencyMap(int type, int firstKey, float baseFrequency)
{
	const size_t mapLen = 128;
	const size_t relLen = 12;
	static float map[mapLen];
	static float relation[1][relLen] =
	{
		{ 1, 1.0667, 1.125, 1.2, 1.25, 1.3333, 1.4, 1.5, 1.6, 1.6667, 1.8, 1.875 }
		//                                                    440 
	};

	float baseMult = baseFrequency / relation[type][9];//440/1.6667
	for (int i = 0; i < relLen; i++)
		relation[type][i] *= baseMult;

	float mult = 1 / 8.f /8;
	for (int i = 0; i < firstKey; i++)
		map[i] = 0;
	for (int i = firstKey; i < mapLen; i++, mult *= ((i - firstKey) % relLen) ? 1 : 2)
		map[i] = relation[type][(i - firstKey) % relLen] * mult;

	return map;
};



MIDIMessageInterpreter::MIDIMessageInterpreter(int nKeyboardSize)
	:nKeyboardSize(nKeyboardSize)
{
}

void MIDIMessageInterpreter::SendMessageToInstrument(
	Instrument* pInstrument,
	MIDIMessage* pMessage,
	bool isFirst,
	bool isLast)
{
	long content = pMessage->getContent();

	short bAftertouch = ((0xFF0000 & content) >> 16);
	short bKey = ((0xFF00 & content) >> 8);
	short bKeyPad = (0xFF & content);


	if (bKeyPad == 144){// keyboard
		if (bAftertouch){//key down
			pInstrument->SetWave(
				pKeyFrequencyMap[bKey],
				bAftertouch / aftertouchMax);
		}
		else{// key up

		}
	}
	else{// sensors

	}
}


MIDIMessageInterpreter::~MIDIMessageInterpreter()
{
}
