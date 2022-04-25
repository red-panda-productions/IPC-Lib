#include "Socket.h"
#include <sstream>
#include <cassert>
#include <thread>
#include <WS2tcpip.h>

/// @brief					  The constructor of the receiving thread
/// @param  p_receiveDataFunc The function that will receive data
ReceivingThread::ReceivingThread(const std::function<void()>& p_receiveDataFunc)
  : m_receiveDataFunc(new std::function<void()>(p_receiveDataFunc))
{
    m_thread = new std::thread(&ReceivingThread::ReceivingLoop, this);
}

/// @brief  Checks whether the thread received a message
/// @return Whether the thread received a message
bool ReceivingThread::ReceivedMessage() const
{
    return m_received;
}

/// @brief Sets the thread in a state to receive a message
void ReceivingThread::Receive()
{
    m_received = false;
    m_receiving = true;
}

void ReceivingThread::Stop()
{
	m_stop = true;
}

/// @brief Resets the internal booleans
void ReceivingThread::Reset()
{
    m_received = false;
}

ReceivingThread::~ReceivingThread()
{
	Stop();
	m_thread->join();
	delete m_thread;
}

/// @brief Loops the thread constantly until its commanded to stop
void ReceivingThread::ReceivingLoop()
{
    while (!m_stop)
    {
        if (!m_receiving) continue;
		try
		{
			(*m_receiveDataFunc)();
			m_received = true;
		}
        catch (std::exception&)
        {
			IPCLIB_WARNING("[IPCLIB] Unexpected exception while receiving data")
        }
		m_receiving = false;
    }
}

/// @brief Receive data asynchronously by activating the receive thread
void Socket::ReceiveDataAsync()
{
    m_receivingThread->Receive();
    m_externalReceive = true;
}

/// @brief Receive data by waiting until data has been written on the socket
void Socket::ReceiveData()
{
    Size = recv(MSocket, DataBuffer, IPC_BUFFER_BYTE_SIZE, 0);
    if (Size == SOCKET_ERROR)
    {
        THROW_IPCLIB_ERROR("[WSA] Failed to receive message. Error code: " << WSAGetLastError());
    }
}

/// @brief Initializes the receive thread
void Socket::Initialize()
{
    if (m_receivingThread)
    {
        THROW_IPCLIB_ERROR("[IPCLib] Socket was already initialized");
    }
    auto function = [this]
    { ReceiveData(); };
    m_receivingThread = new ReceivingThread(function);
}

/// @brief Stops the receive thread
void Socket::Stop()
{
	delete m_receivingThread;
}

/// @brief				Awaits until data has been written to the socket
/// @param p_dataBuffer The data buffer for storing the data
/// @param p_size		The size of the buffer
void Socket::AwaitData(char* p_dataBuffer, int p_size)
{
    if (!m_externalReceive)
    {
        ReceiveData();
        m_internalReceive = true;
        GetData(p_dataBuffer, p_size);
        m_internalReceive = false;
        return;
    }
    while (!m_receivingThread->ReceivedMessage()) {}
    GetData(p_dataBuffer, p_size);
    m_externalReceive = false;
}

/// @brief				Gets the data from the socket, but can receive no data if no data has been written
/// @param p_dataBuffer The data buffer for storing the data
/// @param p_size		The size of the buffer
/// @return				If it received data or not
bool Socket::GetData(char* p_dataBuffer, int p_size)
{
    if (!m_receivingThread->ReceivedMessage() && !m_internalReceive) return false;
    assert(Size >= 0 && Size < p_size);
    for (int i = 0; i < Size; i++)
    {
        p_dataBuffer[i] = DataBuffer[i];
    }
    p_dataBuffer[Size] = '\0';
    if (m_externalReceive) m_receivingThread->Reset();
    return true;
}
