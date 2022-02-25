#pragma once
#include "ipclib_export.h"
#ifndef MESSAGEBYTESIZE
#define MESSAGEBYTESIZE 512
#endif

class IPCLIB_EXPORT ISerializable
{
public:
	virtual ~ISerializable() = default;
	virtual void Serialize(char* message, int& size) = 0;
};

class IPCLIB_EXPORT IDeserializable
{
public:
	virtual ~IDeserializable() = default;
	virtual void Deserialize(char* message, int size) = 0;
};