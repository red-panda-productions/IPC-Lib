#pragma once

#if Win32
#pragma comment(lib, "ws2_32.lib")
#include <WS2tcpip.h>
#include <WinSock2.h>

#endif