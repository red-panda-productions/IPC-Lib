#pragma once
#pragma comment(lib,"ws2_32.lib")
#include "ipclib_export.h"
#include <WinSock2.h>

class IPCLIB_EXPORT ClientSocket
{
public:
	ClientSocket(PCWSTR ip = L"127.0.0.1", int port = 8888);

	void WaitForData(char* dataBuffer, int& size) const;

	void SendData(char* data, int size) const;

	void Disconnect();

	~ClientSocket();

private:
	WSAData m_wsa;
	SOCKET m_socket;
	struct sockaddr_in m_server;
	bool m_disconnected;
};