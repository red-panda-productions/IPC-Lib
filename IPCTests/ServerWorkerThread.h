#include "ServerSocket.h"
#include "TestSerializer.h"
#include <thread>
#include <mutex>

#define DEFAULT_SERVER_MESSAGE "Hello from server"

/// <summary>
/// Worker thread for server
/// </summary>
class ServerWorkerThread : public WorkerThread
{
private:
	ServerSocket m_socket;
public:
	
	/// <summary>
	/// Constructs the message that can be send
	/// </summary>
	ServerWorkerThread() : WorkerThread(DEFAULT_SERVER_MESSAGE)
	{
	}

	/// <summary>
	/// Starts the worker thread
	/// </summary>
	void StartThread()
	{
		std::thread t = std::thread(&ServerWorkerThread::Loop, this);
		t.detach();
	}


protected:

	/// <summary>
	/// Performs an action
	/// </summary>
	/// <param name="action"></param>
	void PerformAction(ACTION action) override
	{
		switch (action)
		{
		case WRITEACTION:
			Message_lock->lock();
			m_socket.SendData(Message);
			Message_lock->unlock();
			return;
		case READACTION:
			Message_lock->lock();
			m_socket.WaitForData(Message);
			MessageWritten = true;
			Message_lock->unlock();
			return;
		case DISCONNECTACTION:
			m_socket.Disconnect();
			return;
		case CLOSESERVERACTION:
			m_socket.CloseServer();
			return;
		case DECONSTRUCTACTION:
			m_socket.~ServerSocket();
			return;
		case CONNECTACTION:
			m_socket.WaitForClientConnection();
			return;
		default:
			return;
		}
	}

};