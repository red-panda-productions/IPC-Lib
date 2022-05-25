#include "ServerSocket.h"
#include "ipclib_portability.h"
#include <sstream>
#include <thread>
#include <stdexcept>

/// @brief Checks if the server is still open
#define CHECK_OPEN() \
    if (!m_open) return IPCLIB_CLOSED_CONNECTION_ERROR;

// ----------------------------------------------- ServerSocketAsync -----------------------------------------------

/// @brief			      The constructor of ServerSocketAsync
/// @param  p_ip		  The IP adress of the server
/// @param  p_port		  The port of the server
/// @param  p_connections The maximum amount of (queued) connections
ServerSocket::ServerSocket(IPC_IP_TYPE p_ip, int p_port, int p_connections)
    : m_ip(p_ip), m_port(p_port), m_connections(p_connections), m_connecting(false), m_server(), m_client()
{
    INET_PTON(AF_INET, p_ip, &m_server.sin_addr.s_addr);
    m_server.sin_family = AF_INET;
    m_server.sin_port = htons(p_port);

    // init client for c++ object purposes
    INET_PTON(AF_INET, p_ip, &m_client.sin_addr.s_addr);
    m_client.sin_family = AF_INET;
    m_client.sin_port = htons(p_port);
}

/// @brief  Initializes the server
/// @return The error code
int ServerSocket::Initialize()
{
    if (m_open) return IPCLIB_SUCCEED;
    int errorCode = StartServer(m_ip, m_port, m_connections, SocketData, m_serverSocket, m_server, m_client, m_open);
    if (errorCode != IPCLIB_SUCCEED)
    {
        CLOSE_SOCKET(m_serverSocket);   //@NOLINUXCOVERAGE, linux uses filedescriptors that can be reused
        CLEANUP_SOCKET();               //@NOLINUXCOVERAGE
        return errorCode;               //@NOLINUXCOVERAGE
    }
    Socket::Initialize();
    return errorCode;
}

/// @brief Connects the server to a client asynchronously
void ServerSocket::ConnectAsync()
{
    m_connecting = true;
    std::thread t(&ServerSocket::ConnectThreadFunction, this);
    t.detach();
}

void ServerSocket::ConnectThreadFunction()
{
    m_connectErrorCode = Connect();
}

/// @brief Connects the server to a client by waiting on a connection
int ServerSocket::Connect()
{
    CHECK_OPEN();

    SOCKET_LENGTH c = sizeof(struct sockaddr_in);
    if ((MSocket = accept(m_serverSocket, (struct sockaddr*)&m_client, &c)) == INVALID_SOCKET)
    {
        IPCLIB_ERROR(SOCKET_LIBRARY_NAME "Bind failed with error code : " << GET_LAST_ERROR(), GET_LAST_ERROR());
    }
    Disconnected = false;
    return IPCLIB_SUCCEED;
}

/// @brief Awaits until a client has connected to the server
int ServerSocket::AwaitClientConnection()
{
    if (m_connecting)
    {
        while (Disconnected) { std::this_thread::yield(); }
        m_connecting = false;
        return m_connectErrorCode;
    }
    return Connect();
}

/// @brief			Sends data to a client
/// @param  p_data	The data that needs to be send
/// @param  p_size  The size of the data
///	@return			The error code
int ServerSocket::SendData(const char* p_data, const int p_size) const
{
    CHECK_OPEN();
    if (Disconnected)
    {
        IPCLIB_ERROR("[IPCLIB] The server was not connected to a client", IPCLIB_SERVER_ERROR)
    }
    if (send(MSocket, p_data, p_size, 0) == SOCKET_ERROR)
    {
        IPCLIB_ERROR(SOCKET_LIBRARY_NAME "Connection error. Error code: " << GET_LAST_ERROR(), SOCKET_LIBRARY_ERROR);
    }
    return IPCLIB_SUCCEED;
}

/// @brief Disconnects the server from the client
int ServerSocket::Disconnect()
{
    CHECK_OPEN();
    CLOSE_SOCKET(MSocket);
    Disconnected = true;
    return IPCLIB_SUCCEED;
}

/// @brief Closes the server and disconnects a client if connected
int ServerSocket::CloseServer()
{
    CHECK_OPEN();
    if (!Disconnected)
    {
        Disconnect();
    }
    CLOSE_SOCKET(m_serverSocket);
    CLEANUP_SOCKET();
    m_open = false;
    return IPCLIB_SUCCEED;
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
    if (Disconnected) return false;
    m_connecting = false;
    return true;
}
