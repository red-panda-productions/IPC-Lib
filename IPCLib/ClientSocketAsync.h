#pragma once
#pragma comment(lib,"ws2_32.lib")
#include "ipclib_export.h"
#include <WinSock2.h>
#include "SocketAsync.h"

#ifndef CLIENT_BUFFER_BYTE_SIZE 
#define CLIENT_BUFFER_BYTE_SIZE 512
#endif

class IPCLIB_EXPORT ClientSocketAsync : public SocketAsync
{
public:
	ClientSocketAsync(PCWSTR p_ip = L"127.0.0.1", int p_port = 8888);

	void SendData(const char* p_data, const int p_size) const;

	void Disconnect();

	~ClientSocketAsync();

private:
	WSAData m_wsa;
	struct sockaddr_in m_server;
};