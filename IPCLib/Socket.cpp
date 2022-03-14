#include "Socket.h"

#include <cassert>
#include <thread>
#include <iostream>
#include <WS2tcpip.h>
#include <iostream>
#include <cstdio>
#include <thread>

/// @brief Receive data asynchronously by spawning a thread
void Socket::ReceiveDataAsync()
{
	m_receiving = true;
	std::thread t(&Socket::ReceiveData, this);
	t.detach();
}

/// @brief Receive data by waiting until data has been written on the socket
void Socket::ReceiveData()
{
	m_size = recv(m_socket, m_dataBuffer, IPC_BUFFER_BYTE_SIZE, 0);
	if (m_size == SOCKET_ERROR)
	{
		printf("Failed to receive message");
		std::cin.get();
		// environment exit? exit(-1);
	}
	m_received = true;
}

/// @brief				Awaits until data has been written to the socket
/// @param p_dataBuffer The data buffer for storing the data
/// @param p_size		The size of the buffer
void Socket::AwaitData(char* p_dataBuffer, int p_size)
{
	if(!m_receiving)
	{
		ReceiveData();
		GetData(p_dataBuffer, p_size);
		return;
	}
	while (!m_received) {}
	GetData(p_dataBuffer, p_size);
}

/// @brief				Gets the data from the socket, but can receive no data if no data has been written
/// @param p_dataBuffer The data buffer for storing the data
/// @param p_size		The size of the buffer
/// @return				If it received data or not
bool Socket::GetData(char* p_dataBuffer, int p_size)
{
	if (!m_received) return false;
	assert(m_size >= 0 && m_size < p_size);
	for(int i = 0; i < m_size; i++)
	{
		p_dataBuffer[i] = m_dataBuffer[i];
	}
	p_dataBuffer[m_size] = '\0';
	m_received = false;
	m_receiving = false;
	return true;
}