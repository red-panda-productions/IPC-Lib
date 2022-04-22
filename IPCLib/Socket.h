#pragma once
#pragma comment(lib,"ws2_32.lib")
#include "ipclib_export.h"
#include <WinSock2.h>
#include <iostream>
#include <thread>
#include <functional>

#define IPC_BUFFER_BYTE_SIZE 512

#define THROW_IPCLIB_ERROR(p_message) \
    std::stringstream oss; \
    oss << p_message; \
    throw std::runtime_error(oss.str());

#define IPCLIB_ERROR(p_message, p_errorCode)\
	std::cerr << p_message << std::endl;  \
	return p_errorCode;

#define IPCLIB_WARNING(p_message, p_errorCode)\
	std::cerr << p_message << std::endl; 

#define WSA_ERROR -1
#define IPCLIB_SERVER_ERROR 1
#define IPCLIB_CLIENT_ERROR 2
#define IPCLIB_SUCCEED 0


/// @brief A worker thread that can be commanded to receive data
class IPCLIB_EXPORT ReceivingThread
{
public:
    explicit ReceivingThread(const std::function<void()>& p_receiveDataFunc);

	bool ReceivedMessage() const;

    void Receive();

	void Stop();

	void Reset();

	~ReceivingThread();

private:
	void ReceivingLoop();

	bool m_receiving = false;
	bool m_received = false;
	bool m_stop = false;

	std::function<void()>* m_receiveDataFunc = nullptr;

	std::thread* m_thread = nullptr;
};

/// @brief A base class for ServerSocket and ClientSocketAsync
class IPCLIB_EXPORT Socket
{
public:
	void ReceiveDataAsync();

	void AwaitData(char* p_dataBuffer, int p_size);

	bool GetData(char* p_dataBuffer, int p_size);

private:
	void ReceiveData();

protected:
	void Initialize();

	void Stop();

	int m_size = IPC_BUFFER_BYTE_SIZE;
	char m_dataBuffer[IPC_BUFFER_BYTE_SIZE] = {'\0'};
	bool m_disconnected = true;
	bool m_internalReceive = false;
	bool m_externalReceive = false;

	SOCKET m_socket = -1;
	WSAData m_wsa = {};

	ReceivingThread* m_receivingThread = nullptr;
};

