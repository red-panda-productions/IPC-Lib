#include "ClientSocket.h"
#include <WS2tcpip.h>
#include <stdexcept>
#include <sstream>

/// @brief Checks if the client is still connected to the server
#define CHECKCONNECTED() if(m_disconnected) throw std::runtime_error("The client was not connected to a server")

/// @brief                  Connects a client to a server
/// @param  p_ip			The IP adress of the server 
/// @param  p_port			The port of the server
/// @param  p_wsa			WSAData from the client
/// @param  p_socket		The socket of the client
/// @param  p_server		The server information
/// @param  p_disconnected	Disconnected bool of the client
void ConnectToServer(const PCWSTR& p_ip, int p_port, WSADATA& p_wsa, SOCKET& p_socket, sockaddr_in& p_server, bool& p_disconnected)
{
	if (WSAStartup(MAKEWORD(2, 2), &p_wsa) != 0)
	{
		IPCLIB_ERROR("[WSA] Failed to initialize WSA. Error code: " << WSAGetLastError());
	}

	if ((p_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		IPCLIB_ERROR("[WSA] Could not create socket. Error code: " << WSAGetLastError());
	}

	InetPtonW(AF_INET, p_ip, &p_server.sin_addr.s_addr);
	p_server.sin_family = AF_INET;
	p_server.sin_port = htons(p_port);

	auto c = connect(p_socket, (struct sockaddr*)&p_server, sizeof(p_server));
	if (c < 0)
	{
		IPCLIB_ERROR("[WSA] Connection error. Error code: " << WSAGetLastError());
	}

	p_disconnected = false;
}

//--------------------------------------------------------------------- ClientSocketAsync.h -------------------------------------------------------

/// @brief			Constructor of ClientSocketAsync
/// @param  p_ip	IP address of the server
/// @param  p_port	The port of the server
ClientSocket::ClientSocket(PCWSTR p_ip, int p_port)
{
	ConnectToServer(p_ip, p_port, m_wsa, m_socket, m_server, m_disconnected);
}

/// @brief			Sends data over a socket to the server
/// @param p_data	Data that needs to be send
/// @param p_size	The size of the data 
void ClientSocket::SendData(const char* p_data, const int p_size) const
{
	CHECKCONNECTED();

	if (send(m_socket, p_data, p_size, 0) == SOCKET_ERROR) 
	{
		IPCLIB_ERROR("[WSA] Connection error. Error code: " << WSAGetLastError());
	}
}

/// @brief Disconnects the client from the server
void ClientSocket::Disconnect()
{
	closesocket(m_socket);
	WSACleanup();
	m_disconnected = true;
}

/// @brief Deconstructs the ClientSocketAsync class
ClientSocket::~ClientSocket()
{
	if (m_disconnected) return;
	Disconnect();
}