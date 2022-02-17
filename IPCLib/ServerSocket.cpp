#include "ServerSocket.h"

void CheckOpen(bool open)
{
	if (!open)
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

ServerSocket::ServerSocket(PCWSTR ip, int port, int connections)
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
	InetPtonW(AF_INET, ip, &m_server.sin_addr.s_addr);
	m_server.sin_family = AF_INET;
	m_server.sin_port = htons(port);

	// init client otherwise vs is wining that it is not initialized
	InetPtonW(AF_INET, ip, &m_client.sin_addr.s_addr);
	m_client.sin_family = AF_INET;
	m_client.sin_port = htons(port);

	if (bind(m_socket, (struct sockaddr*)&m_server, sizeof(m_server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		std::cin.get();
		// environment exit? exit(-1);
	}

	listen(m_socket, connections);

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
/// Waits for data from a client
/// </summary>
/// <param name="data">Expected data format that can be deseralized</param>
void ServerSocket::WaitForData(IDeserializable& data) const
{
	CheckOpen(m_open);
	char message[MESSAGEBYTESIZE];
	int size = recv(m_clientSocket, message, MESSAGEBYTESIZE, 0);
	if (size == SOCKET_ERROR)
	{
		printf("Failed to receive message");
		std::cin.get();
		// environment exit? exit(-1);
	}
	data.Deserialize(message, size);
}

/// <summary>
/// Sends data to the client
/// </summary>
/// <param name="data">The data to be send that can be serialized</param>
void ServerSocket::SendData(ISerializable& data) const
{
	CheckOpen(m_open);
	char message[MESSAGEBYTESIZE]; //chars are bytes, this is put on the stack
	int messageSize = 0;
	data.Serialize(message, messageSize); // needs to end with '\0'
	send(m_clientSocket, message, messageSize, 0);
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