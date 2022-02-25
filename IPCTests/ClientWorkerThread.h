#include "WorkerThread.h"
#include "ClientSocket.h"
#include "TestSerializer.h"
#include <thread>
#include <mutex>

#define DEFAULT_CLIENT_MESSAGE "Hello from client"

/// <summary>
/// Woker thread for client
/// </summary>
class ClientWorkerThread : public WorkerThread
{
private:
	ClientSocket m_socket;
public:

	/// <summary>
	/// Constructs the message that can be send
	/// </summary>
	ClientWorkerThread() : WorkerThread(DEFAULT_CLIENT_MESSAGE)
	{
	}

	/// <summary>
	/// Starts the worker thread
	/// </summary>
	void StartThread()
	{
		std::thread t = std::thread(&ClientWorkerThread::Loop,this);
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
		case DECONSTRUCTACTION:
			m_socket.~ClientSocket();
			return;
		default:
			return;
		}
	}





};