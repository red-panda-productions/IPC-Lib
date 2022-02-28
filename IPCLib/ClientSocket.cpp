#include "ClientSocket.h"
#include "ClientSocketAsync.h"
#include <WS2tcpip.h>
#include <iostream>
#include <stdio.h>
#include <thread>

void ConnectToServer(const PCWSTR& p_ip, int p_port, WSADATA& p_wsa, SOCKET& p_socket, sockaddr_in& p_server, bool& p_disconnected)
{
	if (WSAStartup(MAKEWORD(2, 2), &p_wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		std::cin.get();
		// environment exit? exit(-1);
	}

	if ((p_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
		std::cin.get();
		// environment exit? exit(-1);
	}

	InetPtonW(AF_INET, p_ip, &p_server.sin_addr.s_addr);
	p_server.sin_family = AF_INET;
	p_server.sin_port = htons(p_port);

	auto c = connect(p_socket, (struct sockaddr*)&p_server, sizeof(p_server));
	if (c < 0)
	{
		puts("connect error");
		printf("%d", c);
		std::cin.get();
		// environment exit? exit(-1);
	}

	p_disconnected = false;
}

//------------------------------------------------------------------ ClientSocket.h -------------------------------------------------------------------

/// <summary>
/// Constructor of ClientSocket
/// </summary>
/// <param name="p_ip">Ip address of server</param>
/// <param name="p_port">Port of server</param>
ClientSocket::ClientSocket(PCWSTR p_ip, int p_port)
{
	ConnectToServer(p_ip, p_port,m_wsa,m_socket,m_server,m_disconnected);
}



/// <summary>
/// Waits for data on the socket
/// </summary>
/// <param name="p_dataBuffer">The buffer in which data is received</param>
/// <param name="p_size">The size of the buffer (changes on call)</param>
void ClientSocket::WaitForData(char* p_dataBuffer, int& p_size) const
{
	p_size = recv(m_socket, p_dataBuffer, p_size, 0);
	if (p_size == SOCKET_ERROR)
	{
		printf("Failed to receive message");
		std::cin.get();
		// environment exit? exit(-1);
	}
}

/// <summary>
/// Sends data to the server
/// </summary>
/// <param name="p_data">The data that needs to be send</param>
/// <param name="p_size">The size of the data buffer</param>
void ClientSocket::SendData(char* p_data, int p_size) const
{
	send(m_socket, p_data, p_size, 0);
}

/// <summary>
/// Disconnects from server
/// </summary>
void ClientSocket::Disconnect()
{
	closesocket(m_socket);
	WSACleanup();
	m_disconnected = true;
}

/// <summary>
/// Deconstructor, if it is not disconnected it disconnects from the server
/// </summary>
ClientSocket::~ClientSocket()
{
	if (m_disconnected) return;
	Disconnect();
}

//--------------------------------------------------------------------- ClientSocketAsync.h -------------------------------------------------------

ClientSocketAsync::ClientSocketAsync(PCWSTR p_ip, int p_port)
{
	ConnectToServer(p_ip, p_port, m_wsa, m_socket, m_server, m_disconnected);
}



void ClientSocketAsync::SendData(const char* p_data, const int p_size) const
{
	send(m_socket, p_data, p_size, 0);
}

void ClientSocketAsync::Disconnect()
{
	closesocket(m_socket);
	WSACleanup();
	m_disconnected = true;
}

ClientSocketAsync::~ClientSocketAsync()
{
	if (m_disconnected) return;
	Disconnect();
}