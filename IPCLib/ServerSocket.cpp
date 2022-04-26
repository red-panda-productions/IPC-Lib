#include "ServerSocket.h"
#include <WS2tcpip.h>
#include <sstream>
#include <thread>
#include <stdexcept>

/// @brief Checks if the server is still open
#define CHECK_OPEN() \
    if (!m_open) throw std::runtime_error("[IPCLIB] The server was closed")

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
int StartServer(PCWSTR p_ip, int p_port, int p_connections, WSAData& p_wsa, SOCKET& p_socket, sockaddr_in& p_server, sockaddr_in& p_client, bool& p_open)
{
    if (WSAStartup(MAKEWORD(2, 2), &p_wsa) != 0)
    {
        IPCLIB_ERROR("[WSA] Failed to initialize WSA. Error code: " << WSAGetLastError(), WSA_ERROR);
    }

    if ((p_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        IPCLIB_ERROR("[WSA] Could not create socket : " << WSAGetLastError(), WSA_ERROR);
    }

    if (bind(p_socket, (struct sockaddr*)&p_server, sizeof(p_server)) == SOCKET_ERROR)
    {
        IPCLIB_ERROR("[IPCLib] Bind failed with error code : " << WSAGetLastError(), IPCLIB_SERVER_ERROR);
    }

    listen(p_socket, p_connections);
    p_open = true;
    return IPCLIB_SUCCEED;
}

// ----------------------------------------------- ServerSocketAsync -----------------------------------------------

/// @brief			      The constructor of ServerSocketAsync
/// @param  p_ip		  The IP adress of the server
/// @param  p_port		  The port of the server
/// @param  p_connections The maximum amount of (queued) connections
ServerSocket::ServerSocket(PCWSTR p_ip, int p_port, int p_connections)
    : m_ip(p_ip), m_port(p_port), m_connections(p_connections), m_open(false), m_connecting(false), m_server(), m_client()
{
    InetPtonW(AF_INET, p_ip, &m_server.sin_addr.s_addr);
    m_server.sin_family = AF_INET;
    m_server.sin_port = htons(p_port);

    // init client for c++ object purposes
    InetPtonW(AF_INET, p_ip, &m_client.sin_addr.s_addr);
    m_client.sin_family = AF_INET;
    m_client.sin_port = htons(p_port);
}

/// @brief  Initializes the server
/// @return The error code
int ServerSocket::Initialize()
{
    int errorCode = StartServer(m_ip, m_port, m_connections, Wsa, m_serverSocket, m_server, m_client, m_open);
    if (errorCode != IPCLIB_SUCCEED)
    {
        closesocket(m_serverSocket);
        WSACleanup();
        return errorCode;
    }
    Socket::Initialize();
    return errorCode;
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
    CHECK_OPEN();
    int c = sizeof(struct sockaddr_in);
    if ((MSocket = accept(m_serverSocket, (struct sockaddr*)&m_client, &c)) == INVALID_SOCKET)
    {
        THROW_IPCLIB_ERROR("[WSA] Bind failed with error code : " << WSAGetLastError());
    }
    Disconnected = false;
}

/// @brief Awaits until a client has connected to the server
void ServerSocket::AwaitClientConnection()
{
    if (m_connecting)
    {
        while (Disconnected) { std::this_thread::yield(); }
        m_connecting = false;
        return;
    }
    Connect();
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
        IPCLIB_ERROR("[WSA] Connection error. Error code: " << WSAGetLastError(), WSA_ERROR);
    }
    return IPCLIB_SUCCEED;
}

/// @brief Disconnects the server from the client
void ServerSocket::Disconnect()
{
    CHECK_OPEN();
    closesocket(MSocket);
    Disconnected = true;
}

/// @brief Closes the server and disconnects a client if connected
void ServerSocket::CloseServer()
{
    CHECK_OPEN();
    if (!Disconnected)
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
    if (Disconnected) return false;
    m_connecting = false;
    return true;
}
