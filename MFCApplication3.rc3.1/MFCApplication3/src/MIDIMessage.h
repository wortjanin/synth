#pragma once

class MIDIMessage
{
public:
	enum Type{
		Empty = 0,
		Data = 1,
		Close = 2
	};

public:
	MIDIMessage(Type messageType);
	MIDIMessage(void* pInstance, long content, long timestamp, Type messageType);

	long getContent();


	long getTimestamp();//ms

	Type getMessageType();

	virtual ~MIDIMessage();


private:
	long content;

	long timestamp;

	Type messageType;

	void* pInstance;
};

