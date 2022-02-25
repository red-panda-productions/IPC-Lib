#include "ServerSocket.h"

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

/// <summary>
/// Constructor of ServerSocket
/// </summary>
/// <param name="ip">Ip adress to host server on</param>
/// <param name="port">Port to host server on</param>
/// <param name="connections">Amount of connections that can be handled at the same time</param>

ServerSocket::ServerSocket(PCWSTR p_ip, int p_port, int p_connections)
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsa) != 0)
	{
		printf("Failed WSA startup. Error Code : %d", WSAGetLastError());
		std::cin.get();
		// environment exit? exit(-1);
	}

	if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket: %d", WSAGetLastError());
		std::cin.get();
		// environment exit? exit(-1);
	}
	InetPtonW(AF_INET, p_ip, &m_server.sin_addr.s_addr);
	m_server.sin_family = AF_INET;
	m_server.sin_port = htons(p_port);

	// init client for c++ object purposes
	InetPtonW(AF_INET, p_ip, &m_client.sin_addr.s_addr);
	m_client.sin_family = AF_INET;
	m_client.sin_port = htons(p_port);

	if (bind(m_socket, (struct sockaddr*)&m_server, sizeof(m_server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		std::cin.get();
		// environment exit? exit(-1);
	}

	listen(m_socket, p_connections);

	m_disconnected = true;
	m_open = true;
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