#include "ClientSocket.h"
#include <WS2tcpip.h>
#include <iostream>
#include <stdio.h>

/// <summary>
/// Constructor of ClientSocket
/// </summary>
/// <param name="p_ip">Ip address of server</param>
/// <param name="p_port">Port of server</param>
ClientSocket::ClientSocket(PCWSTR p_ip, int p_port)
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		std::cin.get();
		// environment exit? exit(-1);
	}

	if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
		std::cin.get();
		// environment exit? exit(-1);
	}

	InetPtonW(AF_INET, p_ip, &m_server.sin_addr.s_addr);
	m_server.sin_family = AF_INET;
	m_server.sin_port = htons(p_port);

	auto c = connect(m_socket, (struct sockaddr*)&m_server, sizeof(m_server));
	if (c < 0)
	{
		puts("connect error");
		printf("%d", c);
		std::cin.get();
		// environment exit? exit(-1);
	}

	m_disconnected = false;
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
