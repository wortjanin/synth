#include "../MIDIDevice.h"

#include "../MIDIMessageQueue.h"


#include <Windows.h>
#include <mmsystem.h>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include <map>

static class MidiIn{
public:
	MidiIn(){}

	MidiIn(int dwBufferCount, int dwBufferLength){
		this->dwBufferCount = dwBufferCount;
		this->dwBufferLength = dwBufferLength;

		MidiInHdr = boost::shared_array<MIDIHDR>(new MIDIHDR[dwBufferCount]);
		
		memory = boost::shared_array<char>(new char[dwBufferCount*dwBufferLength]);
		midiData = boost::shared_array<LPSTR>(new LPSTR[dwBufferCount]);
	}

	~MidiIn(){

	}

	HMIDIIN hMidiIn;

	boost::shared_array<MIDIHDR> MidiInHdr;

	boost::shared_array<LPSTR> midiData;
	boost::shared_array<char> memory;
	int dwBufferCount;
	int dwBufferLength;
};

static std::map<const char*, MidiIn> mapDeviceIdMidiIn;

MIDIDevice::MIDIDevice(
	const char* productName, 
	int dwBufferCount, 
	int dwBufferLength,
	int midiMessageQueueCapacity
	)
{
	this->productName = (char*)productName;
	this->dwBufferCount = dwBufferCount;
	this->dwBufferLength = dwBufferLength;

	this->pMIDIMessageQueue = boost::shared_ptr<MIDIMessageQueue>(new MIDIMessageQueue(midiMessageQueueCapacity));
}

static void CALLBACK MidiInCallback(
	HMIDIIN hMidiIn,
	UINT wMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR midiMessage,
	DWORD_PTR timestamp
	){
	auto device = (MIDIDevice *)dwInstance;
	if (wMsg == MIM_DATA){
		device->enque(MIDIMessage((void*)dwInstance, midiMessage, timestamp, MIDIMessage::Data));
	}
	else if (wMsg == MIM_CLOSE){
		device->enque(MIDIMessage(MIDIMessage::Close));
	}
};

void MIDIDevice::run(boost::shared_ptr<Instrument> pInstrument){
	this->pInstrument = pInstrument;
	start(productName, dwBufferCount, dwBufferLength);

}

void MIDIDevice::start(const char* productName, int dwBufferCount, int dwBufferLength){
	boost::mutex::scoped_lock lock(doneMutex);
	if (running)
		return;

	int n = midiInGetNumDevs();
	for (int i = 0; i < n; i++){
		UINT wDeviceID = i;

		MIDIINCAPSW caps;
		UINT r = midiInGetDevCaps(wDeviceID, &caps, sizeof(caps));
		//WCHAR *sMidi = L"USB MIDI Interface";
		std::wstring w(caps.szPname);
		std::string str(w.begin(), w.end());
		if (!strcmp(str.c_str(), productName)){
			MidiIn midiIn(dwBufferCount, dwBufferLength);

			DWORD_PTR dwInstance = (DWORD_PTR)this;
			DWORD_PTR dwCallback = (DWORD_PTR)(&MidiInCallback);
			DWORD dwFlags = CALLBACK_FUNCTION | MIDI_IO_STATUS;

			MMRESULT mmResult = midiInOpen(&midiIn.hMidiIn, wDeviceID, dwCallback, dwInstance, dwFlags);

			for (int b = 0; b < dwBufferCount; b++){
				LPSTR p = &(midiIn.memory[b*dwBufferLength]);
				midiIn.MidiInHdr[b].lpData = &(midiIn.memory[i*dwBufferLength]);
				midiIn.MidiInHdr[b].dwBufferLength = dwBufferLength;
				midiIn.MidiInHdr[b].dwFlags = 0;

				mmResult = midiInPrepareHeader(midiIn.hMidiIn, &midiIn.MidiInHdr[b], sizeof(MIDIHDR));
				mmResult = midiInAddBuffer(midiIn.hMidiIn, &midiIn.MidiInHdr[b], sizeof(MIDIHDR));
			}

			mapDeviceIdMidiIn[productName] = midiIn;

			mmResult = midiInStart(mapDeviceIdMidiIn[productName].hMidiIn);

			pMIDIMessageQueue->reset();
			boost::thread midiThread(&MIDIDevice::MidiInHandler, this);
			midiThread.detach();
		}
	}

}

void MIDIDevice::stop(){
	boost::mutex::scoped_lock lock(doneMutex);
	if (!running)
		return;
	running = false;
	while (!done) stoppedCV.wait(lock);

	try{
		auto midiIn = mapDeviceIdMidiIn.at(productName);//throws exception if the element has been deleted
		MMRESULT mmResult = midiInStop(midiIn.hMidiIn);

		for (int i = 0; i < midiIn.dwBufferCount; i++){
			midiIn.MidiInHdr[i].lpData = NULL;
			midiIn.MidiInHdr[i].dwBufferLength = 0;
			midiIn.MidiInHdr[i].dwFlags = 0;
			mmResult = midiInUnprepareHeader(midiIn.hMidiIn, &midiIn.MidiInHdr[i], sizeof(MIDIHDR));
		}

		mmResult = midiInReset(midiIn.hMidiIn);

		mmResult = midiInClose(midiIn.hMidiIn);

		mapDeviceIdMidiIn.erase(productName);
	}
	catch(...){}
}

void MIDIDevice::enque(MIDIMessage midiMessage){
	pMIDIMessageQueue->enque(midiMessage);
}

void MIDIDevice::MidiInHandler(){
	doneMutex.lock();
	if (running){
		doneMutex.unlock();
		return;
	}

	running = true;
	done = false;
	doneMutex.unlock();
	while (running){
		MIDIMessage m(MIDIMessage::Empty);
		while (running && pMIDIMessageQueue->deque(&m)){
			switch (m.getMessageType()){
			case MIDIMessage::Data:{
				if (pInstrument)
					pInstrument->HandleMIDIMessage(&m);
				break;
			}
			case MIDIMessage::Close:{
				running = false;
				break;
			}
			}
		}
	}

	boost::mutex::scoped_lock lock(doneMutex);
	this->pInstrument = NULL;
	done = true;
	stoppedCV.notify_one();
}


MIDIDevice::~MIDIDevice()
{
	if (running)
		stop();
}

