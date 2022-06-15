#include "Socket.h"
#include "ipclib_portability.h"
#include <sstream>
#include <cassert>
#include <thread>
#include <mutex>

/// @brief Checks if the server is still open
#define CHECK_OPEN() \
    if (!Open) return IPCLIB_CLOSED_CONNECTION_ERROR;

#define ERROR_STATE_INV         0b11101111
#define ERROR_STATE             0b00010000
#define STARTED_RECEIVING_STATE 0b00001000
#define RECEIVED_STATE          0b00000100
#define RECEIVING_STATE         0b00000010
#define STOP_STATE              0b00000001

// @brief Checks if the bit is 1 for unsinged integers
#define GET_ERROR()             ((m_state & ERROR_STATE) > 0)
#define GET_STARTED_RECEIVING() ((m_state & STARTED_RECEIVING_STATE) > 0)
#define GET_RECEIVED()          ((m_state & RECEIVED_STATE) > 0)
#define GET_RECEIVING()         ((m_state & RECEIVING_STATE) > 0)
#define GET_STOP()              ((m_state & STOP_STATE) > 0)

// @brief Sets the bits on a given spot
#define SET_RECEIVING_TRUE() (m_state |= RECEIVING_STATE)
#define SET_STOP_TRUE()      (m_state |= STOP_STATE)
#define SET_ERROR_FALSE()    (m_state &= ERROR_STATE_INV)

// @brief Sets the state to a predefined state
#define SET_RECEIVED_STATE()          (m_state = RECEIVED_STATE)
#define SET_ERROR_STATE()             (m_state = ERROR_STATE)
#define SET_EMPTY_STATE()             (m_state = EMPTY_STATE)
#define SET_STARTED_RECEIVING_STATE() (m_state = RECEIVING_STATE | STARTED_RECEIVING_STATE)

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
    return GET_RECEIVED();
}

/// @brief  Check whether the tread threw an error
/// @return Whether the thread threw an error
bool ReceivingThread::HasError() const
{
    if (!GET_ERROR()) return false;
    return true;
}

/// @brief  Check whether the thread has started receiving
/// @return Whether the thread has started receiving
bool ReceivingThread::StartedReceiving() const
{
    return GET_STARTED_RECEIVING() || GET_RECEIVED();
}

/// @brief  Returns an error code if there was an error in this thread
/// @return The error code
int ReceivingThread::GetErrorCode()
{
    SET_ERROR_FALSE();
    return m_error;
}

/// @brief Sets the thread in a state to receive a message
void ReceivingThread::StartReceive()
{
    SET_RECEIVING_TRUE();
}

/// @brief Stops the receiving thread
void ReceivingThread::Stop()
{
    SET_STOP_TRUE();
}

/// @brief Resets the internal booleans
void ReceivingThread::Reset()
{
    SET_EMPTY_STATE();
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
    while (!GET_STOP())
    {
        if (!GET_RECEIVING())
        {
            std::this_thread::yield();
            continue;
        }
        try
        {
            SET_STARTED_RECEIVING_STATE();
            (*m_receiveDataFunc)();
            SET_RECEIVED_STATE();
        }
        catch (std::exception& e)
        {
            IPCLIB_WARNING("[IPCLIB] Unexpected exception while receiving data: " << e.what())
            m_error = IPCLIB_RECEIVE_ERROR;
            SET_ERROR_STATE();
        }
    }
}

/// @brief Receive data asynchronously by activating the receive thread
void Socket::ReceiveDataAsync()
{
    m_receivingThread->StartReceive();
    m_externalReceive = true;
    while (!m_receivingThread->StartedReceiving())
    {
        std::this_thread::yield();
    }
}

/// @brief           Receive data by waiting until data has been written on the socket
void Socket::ReceiveData()
{
    Size = recv(MSocket, DataBuffer, IPC_BUFFER_BYTE_SIZE, 0);
    if (Size == SOCKET_ERROR)
    {
        THROW_IPCLIB_ERROR(SOCKET_LIBRARY_NAME "Failed to receive message. Error code: " << GET_LAST_ERROR());
    }
    if (Size == 0)
    {
        THROW_IPCLIB_ERROR("[IPCLIB] Received empty message, so the server was closed");
    }
}

/// @brief Initializes the receive thread, can only be called once
void Socket::Initialize()
{
    auto function = [this]()
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
        if (m_receivingThread->HasError())
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
