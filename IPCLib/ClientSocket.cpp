#include "ClientSocket.h"
#include <WS2tcpip.h>
#include <iostream>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <sstream>

#define CHECKCONNECTED() if(m_disconnected) throw std::runtime_error("The client was not connected to a server")

 /// <summary>
 /// Connects a client to a server
 /// </summary>
 /// <param name="p_ip"> The IP adress of the server </param>
 /// <param name="p_port"> The port of the server </param>
 /// <param name="p_wsa"> WSAData from the client </param>
 /// <param name="p_socket"> The socket of the client </param>
 /// <param name="p_server"> The server information </param>
 /// <param name="p_disconnected"> Disconnected bool of the client </param>
void ConnectToServer(const PCWSTR& p_ip, int p_port, WSADATA& p_wsa, SOCKET& p_socket, sockaddr_in& p_server, bool& p_disconnected)
{
	if (WSAStartup(MAKEWORD(2, 2), &p_wsa) != 0)
	{
		std::ostringstream oss;
		oss << "Failed to initialize WSA. Error code: " << WSAGetLastError();
		throw std::runtime_error(oss.str());
	}

	if ((p_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		std::ostringstream oss;
		oss << "Could not create socket. Error code: " << WSAGetLastError();
		throw std::runtime_error(oss.str());
	}

	InetPtonW(AF_INET, p_ip, &p_server.sin_addr.s_addr);
	p_server.sin_family = AF_INET;
	p_server.sin_port = htons(p_port);

	auto c = connect(p_socket, (struct sockaddr*)&p_server, sizeof(p_server));
	if (c < 0)
	{
		std::ostringstream oss;
		oss << "Connection error. Error code: " << WSAGetLastError();
		throw std::runtime_error(oss.str());
	}

	p_disconnected = false;
}

//--------------------------------------------------------------------- ClientSocketAsync.h -------------------------------------------------------

/// <summary>
/// Constructor of ClientSocketAsync
/// </summary>
/// <param name="p_ip"> IP address of the server </param>
/// <param name="p_port"> The port of the server </param>
ClientSocket::ClientSocket(PCWSTR p_ip, int p_port)
{
	ConnectToServer(p_ip, p_port, m_wsa, m_socket, m_server, m_disconnected);
}

/// <summary>
/// Sends data over a socket to the server
/// </summary>
/// <param name="p_data"> Data that needs to be send </param>
/// <param name="p_size"> The size of the data </param>
void ClientSocket::SendData(const char* p_data, const int p_size) const
{
	CHECKCONNECTED();
	send(m_socket, p_data, p_size, 0);
}

/// <summary>
/// Disconnects the client from the server
/// </summary>
void ClientSocket::Disconnect()
{
	closesocket(m_socket);
	WSACleanup();
	m_disconnected = true;
}

/// <summary>
/// Deconstructs the ClientSocketAsync class
/// </summary>
ClientSocket::~ClientSocket()
{
	if (m_disconnected) return;
	Disconnect();
}