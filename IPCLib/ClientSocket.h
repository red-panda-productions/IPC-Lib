#pragma once
#pragma comment(lib,"ws2_32.lib")
#include "ipclib_export.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Serialization.h"
#include <iostream>
#include <stdio.h>

class IPCLIB_EXPORT ClientSocket
{
public:
	ClientSocket(PCWSTR ip = L"127.0.0.1", int port = 8888);

	void WaitForData(IDeserializable& data) const;

	void SendData(ISerializable& data) const;

	void Disconnect();

	~ClientSocket();

private:
	WSAData m_wsa;
	SOCKET m_socket;
	struct sockaddr_in m_server;
	bool m_disconnected;
};