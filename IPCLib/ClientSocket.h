#pragma once
#pragma comment(lib,"ws2_32.lib")
#include "DLLDefines.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Serialization.h"
#include <iostream>
#include <stdio.h>

class SHARED_EXPORT ClientSocket
{
public:
	ClientSocket(PCWSTR ip = L"127.0.0.1", int port = 8888);

	void WaitForData(IDeserializable& data);

	void SendData(ISerializable& data);

	void Disconnect();

	~ClientSocket();

private:
	WSAData m_wsa;
	SOCKET m_socket;
	struct sockaddr_in m_server;
	bool m_disconnected;
};