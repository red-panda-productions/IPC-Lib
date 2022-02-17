#pragma once

#ifndef MESSAGEBYTESIZE
#define MESSAGEBYTESIZE 512
#endif
__interface ISerializable
{
	void Serialize(char* buffer, int& size);
}; 
__interface IDeserializable
{
	void Deserialize(char* message, int size);
};