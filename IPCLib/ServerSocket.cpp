#include "ServerSocket.h"
#include "ServerSocketAsync.h"
#include <WS2tcpip.h>
#include <iostream>
#include <stdio.h>
#include <thread>

/// <summary>
/// Checks if the server is still open
/// </summary>
/// <param name="open"></param>
void CheckOpen(bool p_open)
{
	if (!p_open)
	{
		printf("Server was closed");
		std::cin.get();
		// environment exit? exit(-1);
	}
}

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


//------------------------------------------------------ ServerSocket.h ------------------------------------

/// <summary>
/// Constructor of ServerSocket
/// </summary>
/// <param name="ip">Ip adress to host server on</param>
/// <param name="port">Port to host server on</param>
/// <param name="connections">Amount of connections that can be handled at the same time</param>

ServerSocket::ServerSocket(PCWSTR p_ip, int p_port, int p_connections)
{
	StartServer(p_ip, p_port, p_connections, m_wsa, m_socket, m_server, m_client, m_disconnected, m_open);
}

/// <summary>
/// Waits until a client is connected
/// </summary>
void ServerSocket::WaitForClientConnection()
{
	CheckOpen(m_open);
	int c = sizeof(struct sockaddr_in);

	if ((m_clientSocket = accept(m_socket, (struct sockaddr*)&m_client, &c)) == INVALID_SOCKET)
	{
		printf("Failed to bind with client");
		std::cin.get();
		// environment exit? exit(-1);
	}
	m_disconnected = false;
}

/// <summary>
/// Waits for data from the client
/// </summary>
/// <param name="p_dataBuffer">The buffer in which data is received</param>
/// <param name="p_size">The size of the buffer</param>
void ServerSocket::WaitForData(char* p_dataBuffer, int& p_size) const
{
	CheckOpen(m_open);
	p_size = recv(m_clientSocket, p_dataBuffer, p_size, 0);
	if (p_size == SOCKET_ERROR)
	{
		printf("Failed to receive message");
		std::cin.get();
		// environment exit? exit(-1);
	}
}

/// <summary>
/// Sends data to the client
/// </summary>
/// <param name="p_data">The data that needs to be send</param>
/// <param name="p_size">The size of the data</param>
void ServerSocket::SendData(char* p_data, int p_size) const
{
	CheckOpen(m_open);
	send(m_clientSocket, p_data, p_size, 0);
}

/// <summary>
/// Disconnects the current client from the server
/// </summary>
void ServerSocket::Disconnect()
{
	CheckOpen(m_open);
	closesocket(m_clientSocket);
	m_disconnected = true;
}

/// <summary>
/// Closes the server
/// </summary>
void ServerSocket::CloseServer()
{
	CheckOpen(m_open);
	if (!m_disconnected)
	{
		Disconnect();
	}
	closesocket(m_socket);
	WSACleanup();
	m_open = false;
}

/// <summary>
/// Deconstructor, if the server is still open it will close the server
/// </summary>
ServerSocket::~ServerSocket()
{
	if (!m_open) return;
	CloseServer();
}

// ----------------------------------------------- ServerSocketAsync -----------------------------------------------

ServerSocketAsync::ServerSocketAsync(PCWSTR p_ip, int p_port, int p_connections)
{
	StartServer(p_ip, p_port, p_connections, m_wsa, m_socket, m_server, m_client, m_disconnected, m_open);
}

void ServerSocketAsync::ConnectAsync()
{
	std::thread t(&ServerSocketAsync::Connect, this);
	t.detach();
}

void ServerSocketAsync::Connect()
{
	CheckOpen(m_open);
	int c = sizeof(struct sockaddr_in);

	if ((m_clientSocket = accept(m_socket, (struct sockaddr*)&m_client, &c)) == INVALID_SOCKET)
	{
		printf("Failed to bind with client");
		std::cin.get();
		// environment exit? exit(-1);
	}
	m_disconnected = false;
}

void ServerSocketAsync::AwaitClientConnection()
{
	while (m_disconnected) {}
}

void ServerSocketAsync::ReceiveDataAsync()
{
	std::thread t(&ServerSocketAsync::ReceiveData, this);
	t.detach();
}

void ServerSocketAsync::ReceiveData()
{
	m_size = recv(m_socket, m_dataBuffer, SERVER_BUFFER_BYTE_SIZE, 0);
	if (m_size == SOCKET_ERROR)
	{
		printf("Failed to receive message");
		std::cin.get();
		// environment exit? exit(-1);
	}
	m_received = true;
}

void ServerSocketAsync::AwaitData(char* p_dataBuffer, int& p_size)
{
	while (!m_received) {}
	GetData(p_dataBuffer, p_size);
}

bool ServerSocketAsync::GetData(char* p_dataBuffer, int& p_size)
{
	if (!m_received) return false;
	strcpy_s(p_dataBuffer, p_size, m_dataBuffer);
	m_received = false;
	return true;
}

void ServerSocketAsync::SendData(char* p_data, int p_size)
{
	CheckOpen(m_open);
	send(m_clientSocket, p_data, p_size, 0);
}

void ServerSocketAsync::Disconnect()
{
	CheckOpen(m_open);
	closesocket(m_clientSocket);
	m_disconnected = true;
}

void ServerSocketAsync::CloseServer()
{
	CheckOpen(m_open);
	if (!m_disconnected)
	{
		Disconnect();
	}
	closesocket(m_socket);
	WSACleanup();
	m_open = false;
}

ServerSocketAsync::~ServerSocketAsync()
{
	if (!m_open) return;
	CloseServer();
}


