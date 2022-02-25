#pragma once
#pragma comment(lib,"ws2_32.lib")
#include "ipclib_export.h"
#include <WinSock2.h>

/// <summary>
/// Class that represents a client that connects to a socket
/// </summary>
class IPCLIB_EXPORT ClientSocket
{
public:
	ClientSocket(PCWSTR p_ip = L"127.0.0.1", int p_port = 8888);

	void WaitForData(char* p_dataBuffer, int& p_size) const;

	void SendData(char* p_data, int p_size) const;

	void Disconnect();

	~ClientSocket();

private:
	WSAData m_wsa;
	SOCKET m_socket;
	struct sockaddr_in m_server;
	bool m_disconnected;
};

void ConnectToServer(const PCWSTR& p_ip, int p_port);
