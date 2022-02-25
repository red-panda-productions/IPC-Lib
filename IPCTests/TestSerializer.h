#pragma once
#include "Serialization.h"

class TestSerializer : public ISerializable, public IDeserializable
{
public:
	char Message[MESSAGEBYTESIZE];
	int Size;

	/// <summary>
	/// Constructs the message
	/// </summary>
	TestSerializer()
	{
		Size = 0;
	}

	/// <summary>
	/// Constructor for a complete message
	/// </summary>
	/// <param name="message">The message array</param>
	/// <param name="size">the size of the message</param>
	TestSerializer(char* message, int size)
	{
		Deserialize(message, size);
	}

	/// <summary>
	/// Serializes this message to send it over a socket
	/// </summary>
	/// <param name="buffer">the socket buffer</param>
	/// <param name="size">the size of the message (needs to be set)</param>
	void Serialize(char* buffer, int& size) override
	{
		size = this->Size;
		for (int i = 0; i < Size; i++)
		{
			buffer[i] = this->Message[i];
		}
	}

	/// <summary>
	/// Deserializes a message from the socket
	/// </summary>
	/// <param name="message">the message</param>
	/// <param name="size">the size of the message</param>
	void Deserialize(char* message, int size) override
	{
		for (int i = 0; i < size; i++)
		{
			this->Message[i] = message[i];
		}
		this->Size = size;
	}
};
