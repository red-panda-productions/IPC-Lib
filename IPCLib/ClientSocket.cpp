#include "ClientSocket.h"
#include <WS2tcpip.h>
#include <stdexcept>
#include <sstream>

/// @brief Checks if the server is still open
#define CHECK_OPEN() \
    if (Disconnected) return IPCLIB_CLOSED_CONNECTION_ERROR;

/// @brief                  Connects a client to a server
/// @param  p_ip			The IP adress of the server
/// @param  p_port			The port of the server
/// @param  p_wsa			WSAData from the client
/// @param  p_socket		The socket of the client
/// @param  p_server		The server information
/// @param  p_disconnected	Disconnected bool of the client
int ConnectToServer(const PCWSTR& p_ip, int p_port, WSADATA& p_wsa, SOCKET& p_socket, sockaddr_in& p_server, bool& p_disconnected)
{
    if (WSAStartup(MAKEWORD(2, 2), &p_wsa) != 0)
    {
        IPCLIB_ERROR("[WSA] Failed to initialize WSA. Error code: " << WSAGetLastError(), WSA_ERROR);
    }

    if ((p_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        IPCLIB_ERROR("[WSA] Could not create socket. Error code: " << WSAGetLastError(), WSA_ERROR);
    }

    InetPtonW(AF_INET, p_ip, &p_server.sin_addr.s_addr);
    p_server.sin_family = AF_INET;
    p_server.sin_port = htons(p_port);

    auto c = connect(p_socket, (struct sockaddr*)&p_server, sizeof(p_server));
    if (c < 0)
    {
        IPCLIB_ERROR("[WSA] Connection error. Error code: " << WSAGetLastError(), WSA_ERROR);
    }

    p_disconnected = false;
    return IPCLIB_SUCCEED;
}

//--------------------------------------------------------------------- ClientSocketAsync.h -------------------------------------------------------

/// @brief			Constructor of ClientSocketAsync
/// @param  p_ip	IP address of the server
/// @param  p_port	The port of the server
ClientSocket::ClientSocket(PCWSTR p_ip, int p_port)
    : m_ip(p_ip), m_port(p_port), m_server()
{
}

/// @brief  Initializes the client
/// @return The error code
int ClientSocket::Initialize()
{
    if (!Disconnected) return IPCLIB_SUCCEED;
    int errorCode = ConnectToServer(m_ip, m_port, Wsa, MSocket, m_server, Disconnected);
    if (errorCode != IPCLIB_SUCCEED)
    {
        closesocket(MSocket);
        WSACleanup();
        return errorCode;
    }
    Socket::Initialize();
    return IPCLIB_SUCCEED;
}

/// @brief			Sends data over a socket to the server
/// @param p_data	Data that needs to be send
/// @param p_size	The size of the data
int ClientSocket::SendData(const char* p_data, const int p_size) const
{
    if (Disconnected)
    {
        IPCLIB_ERROR("[IPCLIB] The client was not connected to a server", IPCLIB_CLOSED_CONNECTION_ERROR);
    }
    if (send(MSocket, p_data, p_size, 0) == SOCKET_ERROR)
    {
        IPCLIB_ERROR("[WSA] Connection error. Error code: " << WSAGetLastError(), WSA_ERROR);
    }
    return IPCLIB_SUCCEED;
}

/// @brief Disconnects the client from the server
int ClientSocket::Disconnect()
{
    CHECK_OPEN();
    closesocket(MSocket);
    WSACleanup();
    Stop();
    Disconnected = true;
    return IPCLIB_SUCCEED;
}

/// @brief Deconstructs the ClientSocketAsync class
ClientSocket::~ClientSocket()
{
    if (Disconnected) return;
    Disconnect();
}