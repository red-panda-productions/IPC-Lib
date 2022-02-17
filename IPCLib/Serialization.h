#pragma once
#include "DLLDefines.h"
#ifndef MESSAGEBYTESIZE
#define MESSAGEBYTESIZE 512
#endif
__interface SHARED_EXPORT ISerializable
{
	void Serialize(char* buffer, int& size);
}; 
__interface IDeserializable
{
	void Deserialize(char* message, int size);
};