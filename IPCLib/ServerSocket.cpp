#include "ServerSocket.h"
#include <WS2tcpip.h>
#include <iostream>
#include <cstdio>
#include <thread>
#include <stdexcept>
#include <sstream>

/// @brief Checks if the server is still open
#define CHECKOPEN() if(!m_open) throw std::runtime_error("The server was closed")

/// @brief Checks if the server is still connected
 #define CHECKCONNECTED() if(m_disconnected) throw std::runtime_error("The server was not connected to a client")

/// @brief					Starts the server
/// @param  p_ip			The IP adress to start the server on
/// @param  p_port			The port of the server
/// @param  p_connections	The maximum amount of allowed (queued) connections
/// @param  p_wsa			The WSAData of th server
/// @param  p_socket		The socket of the server
/// @param  p_server		The info of the server 
/// @param  p_client		The info of the client (only for initialization)
/// @param  p_disconnected  The disconnected bool of the server
/// @param  p_open			The open bool of the server
void StartServer(PCWSTR p_ip, int p_port, int p_connections, WSAData& p_wsa, SOCKET& p_socket, sockaddr_in& p_server, sockaddr_in& p_client, bool& p_disconnected, bool& p_open)
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
		oss << "Could not create socket : " << WSAGetLastError();
		throw std::runtime_error(oss.str());
	}
	InetPtonW(AF_INET, p_ip, &p_server.sin_addr.s_addr);
	p_server.sin_family = AF_INET;
	p_server.sin_port = htons(p_port);

	// init client for c++ object purposes
	InetPtonW(AF_INET, p_ip, &p_client.sin_addr.s_addr);
	p_client.sin_family = AF_INET;
	p_client.sin_port = htons(p_port);

	if (bind(p_socket, (struct sockaddr*)&p_server, sizeof(p_server)) == SOCKET_ERROR)
	{
		std::ostringstream oss;
		oss << "Bind failed with error code : " << WSAGetLastError();
		throw std::runtime_error(oss.str());
	}

	listen(p_socket, p_connections);

	p_disconnected = true;
	p_open = true;
}


// ----------------------------------------------- ServerSocketAsync -----------------------------------------------

/// @brief			      The constructor of ServerSocketAsync
/// @param  p_ip		  The IP adress of the server
/// @param  p_port		  The port of the server
/// @param  p_connections The maximum amount of (queued) connections
ServerSocket::ServerSocket(PCWSTR p_ip, int p_port, int p_connections)
{
	StartServer(p_ip, p_port, p_connections, m_wsa, m_serverSocket, m_server, m_client, m_disconnected, m_open);
}

/// @brief Connects the server to a client asynchronously
void ServerSocket::ConnectAsync()
{
	m_connecting = true;
	std::thread t(&ServerSocket::Connect, this);
	t.detach();
}

/// @brief Connects the server to a client by waiting on a connection
void ServerSocket::Connect()
{
	CHECKOPEN();
	int c = sizeof(struct sockaddr_in);
	if ((m_socket = accept(m_serverSocket, (struct sockaddr*)&m_client, &c)) == INVALID_SOCKET)
	{
		std::ostringstream oss;
		oss << "Bind failed with error code : " << WSAGetLastError();
		throw std::runtime_error(oss.str());
	}
	m_disconnected = false;
}

/// @brief Awaits until a client has connected to the server
void ServerSocket::AwaitClientConnection()
{
	if(m_connecting)
	{
		while (m_disconnected) {}
		m_connecting = false;
		return;
	}
	Connect();
}

/// @brief			Sends data to a client
/// @param  p_data	The data that needs to be send
/// @param  p_size  The size of the data
void ServerSocket::SendData(const char* p_data, const int p_size) const
{
	CHECKOPEN();
	CHECKCONNECTED();
	if (send(m_socket, p_data, p_size, 0) == SOCKET_ERROR)
	{
		std::ostringstream oss;
		oss << "Connection error. Error code: " << WSAGetLastError();
		throw std::runtime_error(oss.str());
	};
}

/// @brief Disconnects the server from the client
void ServerSocket::Disconnect()
{
	CHECKOPEN();
	closesocket(m_socket);
	m_disconnected = true;
}

/// @brief Closes the server and disconnects a client if connected
void ServerSocket::CloseServer()
{
	CHECKOPEN();
	if (!m_disconnected)
	{
		Disconnect();
	}
	closesocket(m_serverSocket);
	WSACleanup();
	m_open = false;
}

/// @brief Deconstructs the server
ServerSocket::~ServerSocket()
{
	if (!m_open) return;
	CloseServer();
}

/// @brief  Whether the server is connected to a client 
/// @return Deconstructs the server
bool ServerSocket::Connected()
{
	if (m_disconnected) return false;
	m_connecting = false;
	return true;
}
