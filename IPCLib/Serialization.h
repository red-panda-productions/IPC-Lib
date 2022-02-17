#pragma once
#include "ipclib_export.h"
#ifndef MESSAGEBYTESIZE
#define MESSAGEBYTESIZE 512
#endif
__interface IPCLIB_EXPORT ISerializable
{
	void Serialize(char* buffer, int& size);
}; 
__interface IDeserializable
{
	void Deserialize(char* message, int size);
};