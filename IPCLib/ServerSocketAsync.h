#pragma once
#pragma comment(lib,"ws2_32.lib")
#include "ipclib_export.h"
#include <WinSock2.h>

#ifndef SERVER_BUFFER_BYTE_SIZE
#define SERVER_BUFFER_BYTE_SIZE 512
#endif

class IPCLIB_EXPORT ServerSocketAsync 
{
public:
	ServerSocketAsync(PCWSTR p_ip = L"127.0.0.1", int p_port = 8888, int p_connections = 1);

	void ConnectAsync();

	void AwaitClientConnection();

	void ReceiveDataAsync();

	void AwaitData(char* p_dataBuffer, int& p_size);

	bool GetData(char* p_dataBuffer, int& p_size);

	void SendData(char* p_data, int p_size);

	void Disconnect();

	void CloseServer();

	~ServerSocketAsync();

private:

	void ReceiveData();

	void Connect();

	bool m_received = false;
	WSAData m_wsa;
	SOCKET m_socket;
	SOCKET m_clientSocket = 0;
	struct sockaddr_in m_client;
	struct sockaddr_in m_server;

	bool m_disconnected;
	bool m_open;

	int m_size;
	char m_dataBuffer[SERVER_BUFFER_BYTE_SIZE];
};