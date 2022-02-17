#pragma once
#include "Serialization.h"

class TestSerializer : public ISerializable, public IDeserializable
{
public:
	char* m_message;
	int m_size;

	/// <summary>
	/// Constructs the message
	/// </summary>
	TestSerializer()
	{
		m_message = NULL;
		m_size = 0;
	}

	/// <summary>
	/// Constructor for a complete message
	/// </summary>
	/// <param name="message">The message array</param>
	/// <param name="size">the size of the message</param>
	TestSerializer(char* message, int size)
	{
		m_message = message;
		m_size = size;
	}

	/// <summary>
	/// Serializes this message to send it over a socket
	/// </summary>
	/// <param name="buffer">the socket buffer</param>
	/// <param name="size">the size of the message (needs to be set)</param>
	void Serialize(char* buffer, int& size)
	{
		size = this->m_size;
		for (int i = 0; i < m_size; i++)
		{
			buffer[i] = this->m_message[i];
		}
	}

	/// <summary>
	/// Deserializes a message from the socket
	/// </summary>
	/// <param name="message">the message</param>
	/// <param name="size">the size of the message</param>
	void Deserialize(char* message, int size)
	{
		for (int i = 0; i < size; i++)
		{
			this->m_message[i] = message[i];
		}
	}
};
