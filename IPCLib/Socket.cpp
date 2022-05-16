#include "Socket.h"
#include <sstream>
#include <cassert>
#include <thread>

/// @brief Checks if the server is still open
#define CHECK_OPEN() \
    if (!Open) return IPCLIB_CLOSED_CONNECTION_ERROR;

/// @brief					  The constructor of the receiving thread
/// @param  p_receiveDataFunc The function that will receive data
ReceivingThread::ReceivingThread(const std::function<void()>& p_receiveDataFunc)
    : m_receiveDataFunc(new std::function<void()>(p_receiveDataFunc))
{
    m_thread = new std::thread(&ReceivingThread::ReceivingLoop, this);
}

/// @brief  Checks whether the thread received a message
/// @return Whether the thread received a message
bool ReceivingThread::HasReceivedMessage() const
{
    return m_received;
}

/// @brief  Returns an error code if there was an error in this thread
/// @return The error code
int ReceivingThread::GetErrorCode() const
{
    return m_error;
}

/// @brief Sets the thread in a state to receive a message
void ReceivingThread::StartReceive()
{
    m_received = false;
    m_receiving = true;
}

/// @brief Stops the receiving thread
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

/// @brief Loops the thread constantly until it is commanded to stop
void ReceivingThread::ReceivingLoop()
{
    while (!m_stop)
    {
        if (!m_receiving)
        {
            std::this_thread::yield();
            continue;
        }
        try
        {
            (*m_receiveDataFunc)();
            m_received = true;
        }
        catch (std::exception& e)
        {
            IPCLIB_WARNING("[IPCLIB] Unexpected exception while receiving data: " << e.what())
            m_error = IPCLIB_RECEIVE_ERROR;
        }
        m_receiving = false;
    }
}

/// @brief Receive data asynchronously by activating the receive thread
void Socket::ReceiveDataAsync()
{
    m_receivingThread->StartReceive();
    m_externalReceive = true;
}

/// @brief Receive data by waiting until data has been written on the socket
void Socket::ReceiveData()
{
    Size = recv(MSocket, DataBuffer, IPC_BUFFER_BYTE_SIZE, 0);
    if (Size == SOCKET_ERROR)
    {
        THROW_IPCLIB_ERROR("[WSA] Failed to receive message. Error code: " << GET_LAST_ERROR());
    }
    if (Size == 0)
    {
        THROW_IPCLIB_ERROR("[IPCLIB] Received empty message, so the server was closed");
    }
}

/// @brief Initializes the receive thread, can only be called once
void Socket::Initialize()
{
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
/// @return             An error code
int Socket::AwaitData(char* p_dataBuffer, int p_size)
{
    if (!m_externalReceive)
    {
        ReceiveData();
        m_internalReceive = true;
        GetData(p_dataBuffer, p_size);
        m_internalReceive = false;
        return IPCLIB_SUCCEED;
    }
    while (!m_receivingThread->HasReceivedMessage())
    {
        if (m_receivingThread->GetErrorCode() != IPCLIB_SUCCEED)
        {
            return m_receivingThread->GetErrorCode();
        }
        std::this_thread::yield();
    }
    GetData(p_dataBuffer, p_size);
    m_externalReceive = false;
    return IPCLIB_SUCCEED;
}

/// @brief				Gets the data from the socket, but can receive no data if no data has been written
/// @param p_dataBuffer The data buffer for storing the data
/// @param p_size		The size of the buffer
/// @return				If it received data or not
bool Socket::GetData(char* p_dataBuffer, int p_size)
{
    if (!m_receivingThread->HasReceivedMessage() && !m_internalReceive) return false;
    assert(Size >= 0 && Size < p_size);
    for (int i = 0; i < Size; i++)
    {
        p_dataBuffer[i] = DataBuffer[i];
    }
    p_dataBuffer[Size] = '\0';
    if (m_externalReceive) m_receivingThread->Reset();
    return true;
}
