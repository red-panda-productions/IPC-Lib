#include "ServerSocket.h"
#include <WS2tcpip.h>
#include <iostream>
#include <cstdio>
#include <thread>
#include <stdexcept>

/// <summary>
/// Checks if the server is still open
/// </summary>
/// <param name="open"></param>
#define CHECKOPEN() if(!m_open) throw std::runtime_error("The server was closed")
#define CHECKCONNECTED() if(m_disconnected) throw std::runtime_error("The server was not connected to a client")

/// <summary>
/// Starts the server
/// </summary>
/// <param name="p_ip"> The IP adress to start the server on </param>
/// <param name="p_port"> The port of the server </param>
/// <param name="p_connections"> The maximum amount of allowed (queued) connections </param>
/// <param name="p_wsa"> The WSAData of th server </param>
/// <param name="p_socket"> The socket of the server </param>
/// <param name="p_server"> The info of the server </param>
/// <param name="p_client"> The info of the client (only for initialization) </param>
/// <param name="p_disconnected"> The disconnected bool of the server </param>
/// <param name="p_open"> The open bool of the server </param>
void StartServer(PCWSTR p_ip, int p_port, int p_connections, WSAData& p_wsa, SOCKET& p_socket, sockaddr_in& p_server, sockaddr_in& p_client, bool& p_disconnected, bool& p_open)
{
	if (WSAStartup(MAKEWORD(2, 2), &p_wsa) != 0)
	{
		printf("Failed WSA startup. Error Code : %d", WSAGetLastError());
		std::cin.get();
		// environment exit? exit(-1);
	}

	if ((p_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket: %d", WSAGetLastError());
		std::cin.get();
		// environment exit? exit(-1);
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
		printf("Bind failed with error code : %d", WSAGetLastError());
		std::cin.get();
		// environment exit? exit(-1);
	}

	listen(p_socket, p_connections);

	p_disconnected = true;
	p_open = true;
}

// ----------------------------------------------- ServerSocketAsync -----------------------------------------------

 /// <summary>
 /// The constructor of ServerSocketAsync
 /// </summary>
 /// <param name="p_ip"> The IP adress of the server </param>
 /// <param name="p_port"> The port of the server </param>
 /// <param name="p_connections"> The maximum amount of (queued) connections </param>
ServerSocket::ServerSocket(PCWSTR p_ip, int p_port, int p_connections)
{
	StartServer(p_ip, p_port, p_connections, m_wsa, m_serverSocket, m_server, m_client, m_disconnected, m_open);
}

/// <summary>
/// Connects the server to a client asynchronously
/// </summary>
void ServerSocket::ConnectAsync()
{
	m_connecting = true;
	std::thread t(&ServerSocket::Connect, this);
	t.detach();
}

/// <summary>
/// Connects the server to a client by waiting on a connection
/// </summary>
void ServerSocket::Connect()
{
	CHECKOPEN();
	int c = sizeof(struct sockaddr_in);
	if ((m_socket = accept(m_serverSocket, (struct sockaddr*)&m_client, &c)) == INVALID_SOCKET)
	{
		printf("Failed to bind with client");
		std::cin.get();
		// environment exit? exit(-1);
	}
	m_disconnected = false;
}

/// <summary>
/// Awaits until a client has connected to the server
/// </summary>
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

/// <summary>
/// Sends data to a client
/// </summary>
/// <param name="p_data"> The data that needs to be send </param>
/// <param name="p_size"> The size of the data </param>
void ServerSocket::SendData(const char* p_data, const int p_size) const
{
	CHECKOPEN();
	CHECKCONNECTED();
	send(m_socket, p_data, p_size, 0);
}

/// <summary>
/// Disconnects the server from the client
/// </summary>
void ServerSocket::Disconnect()
{
	CHECKOPEN();
	closesocket(m_socket);
	m_disconnected = true;
}

/// <summary>
/// Closes the server and disconnects a client if connected
/// </summary>
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

/// <summary>
/// Deconstructs the server
/// </summary>
ServerSocket::~ServerSocket()
{
	if (!m_open) return;
	CloseServer();
}


/// <returns> Whether the server is connected to a client </returns>
bool ServerSocket::Connected()
{
	if (m_disconnected) return false;
	m_connecting = false;
	return true;
}
