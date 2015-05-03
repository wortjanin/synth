#include "MIDIMessage.h"

MIDIMessage::MIDIMessage(Type messageType){
	this->messageType = messageType;
}

MIDIMessage::MIDIMessage(void* pInstance, long content, long timestamp, Type messageType)
{
	this->pInstance = pInstance;

	this->content = content;

	this->timestamp = timestamp;

	this->messageType = messageType;
}

long MIDIMessage::getContent(){
	return content;
}

long MIDIMessage::getTimestamp(){
	return timestamp;
}

MIDIMessage::Type MIDIMessage::getMessageType(){
	return messageType;
}

MIDIMessage::~MIDIMessage()
{
}

