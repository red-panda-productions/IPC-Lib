#ifdef WIN32
#include "ipclib_portability.h"

/// @brief                  Connects a client to a server
/// @param  p_ip			The IP adress of the server
/// @param  p_port			The port of the server
/// @param  p_wsa			WSAData from the client
/// @param  p_socket		The socket of the client
/// @param  p_server		The server information
/// @param  p_disconnected	Disconnected bool of the client
int ConnectToServer(PCWSTR p_ip, int p_port, WSA_DATA& p_data, SOCKET& p_socket, sockaddr_in& p_server, bool& p_disconnected)
{
    if (WSAStartup(MAKEWORD(2, 2), &p_data) != 0)
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
int StartServer(IPC_IP_TYPE p_ip, int p_port, int p_connections, IPC_DATA_TYPE& p_wsa, SOCKET& p_socket, sockaddr_in& p_server, sockaddr_in& p_client, bool& p_open)
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

#endif