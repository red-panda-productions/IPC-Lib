#pragma once
#pragma comment(lib,"ws2_32.lib")
#include "ipclib_export.h"
#include <WinSock2.h>

#ifndef CLIENT_BUFFER_BYTE_SIZE 
#define CLIENT_BUFFER_BYTE_SIZE 512
#endif

class IPCLIB_EXPORT ClientSocketAsync
{
public:
	ClientSocketAsync(PCWSTR p_ip = L"127.0.0.1", int p_port = 8888);

	void ReceiveDataAsync();
	
	void AwaitData(char* p_dataBuffer, int& p_size);

	bool GetData(char* p_dataBuffer, int& p_size);

	void SendData(char* p_data, int p_size);

	void Disconnect();

	~ClientSocketAsync();

private:

	void ReceiveData();

	bool m_received = false;
	WSAData m_wsa;
	SOCKET m_socket;
	struct sockaddr_in m_server;
	bool m_disconnected;

	int m_size;
	char m_dataBuffer[CLIENT_BUFFER_BYTE_SIZE];
};