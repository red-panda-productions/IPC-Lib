#pragma once
#pragma comment(lib,"ws2_32.lib")
#include "ipclib_export.h"
#include <WinSock2.h>

class IPCLIB_EXPORT ServerSocket
{
public:
	ServerSocket(PCWSTR p_ip = L"127.0.0.1", int p_port = 8888, int p_connections = 1);

	void WaitForClientConnection();

	void WaitForData(char* p_dataBuffer, int& p_size) const;

	void SendData(char* p_data, int p_size) const;

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