#pragma once
#pragma comment(lib,"ws2_32.lib")
#include "DLLDefines.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Serialization.h"
#include <iostream>
#include <stdio.h>

class SHARED_EXPORT ServerSocket
{
public:
	ServerSocket(PCWSTR ip = L"127.0.0.1", int port = 8888, int connections = 1);

	void WaitForClientConnection();

	void WaitForData(IDeserializable& data) const;

	void SendData(ISerializable& data) const;

	void Disconnect();

	void CloseServer();

	~ServerSocket();
private:
	WSAData m_wsa;
	SOCKET m_socket;
	SOCKET m_clientSocket = 0;
	struct sockaddr_in m_client;
	struct sockaddr_in m_server;

	bool m_disconnected;
	bool m_open;
};