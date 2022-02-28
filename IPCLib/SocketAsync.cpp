#include "SocketAsync.h"
#include <thread>
#include <iostream>
#include <WS2tcpip.h>
#include <iostream>
#include <cstdio>
#include <thread>

void SocketAsync::ReceiveDataAsync()
{
	std::thread t(&SocketAsync::ReceiveData, this);
	t.detach();
}

void SocketAsync::ReceiveData()
{
	m_receiving = true;
	m_size = recv(m_socket, m_dataBuffer, BUFFER_BYTE_SIZE, 0);
	if (m_size == SOCKET_ERROR)
	{
		printf("Failed to receive message");
		std::cin.get();
		// environment exit? exit(-1);
	}
	m_received = true;
	m_receiving = false;
}

void SocketAsync::AwaitData(char* p_dataBuffer, int& p_size)
{
	if(!m_receiving)
	{
		printf("ReceiveData was not called");
		std::cin.get();
		// environment exit? exit(-1);
	}
	while (!m_received) {}
	GetData(p_dataBuffer, p_size);
}

bool SocketAsync::GetData(char* p_dataBuffer, int& p_size)
{
	if (!m_received) return false;
	strcpy_s(p_dataBuffer, p_size, m_dataBuffer);
	m_received = false;
	return true;
}