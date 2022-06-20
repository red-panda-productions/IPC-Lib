/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#include "ClientSocket.h"
#include "ipclib_portability.h"
#include <stdexcept>
#include <sstream>

/// @brief Checks if the server is still open
#define CHECK_OPEN() \
    if (Disconnected) return IPCLIB_CLOSED_CONNECTION_ERROR;

/// @brief			Constructor of ClientSocketAsync
/// @param  p_ip	IP address of the server
/// @param  p_port	The port of the server
ClientSocket::ClientSocket(IPC_IP_TYPE p_ip, int p_port)
    : m_ip(p_ip), m_port(p_port), m_server()
{
}

/// @brief  Initializes the client
/// @return The error code
int ClientSocket::Initialize()
{
    if (!Disconnected) return IPCLIB_SUCCEED;
    int errorCode = ConnectToServer(m_ip, m_port, SocketData, MSocket, m_server, Disconnected);
    if (errorCode != IPCLIB_SUCCEED)
    {
        CLOSE_SOCKET(MSocket);
        CLEANUP_SOCKET();
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
        IPCLIB_ERROR(SOCKET_LIBRARY_NAME "Connection error. Error code: " << GET_LAST_ERROR(), SOCKET_LIBRARY_ERROR);
    }
    return IPCLIB_SUCCEED;
}

/// @brief Disconnects the client from the server
int ClientSocket::Disconnect()
{
    CHECK_OPEN();
    CLOSE_SOCKET(MSocket);
    CLEANUP_SOCKET();
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