#pragma once
#pragma comment(lib,"ws2_32.lib")
#include "ipclib_export.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <stdio.h>

class IPCLIB_EXPORT ServerSocket
{
public:
	ServerSocket(PCWSTR ip = L"127.0.0.1", int port = 8888, int connections = 1);

	void WaitForClientConnection();

	void WaitForData(char* dataBuffer, int& size) const;

	void SendData(char* data, int size) const;

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