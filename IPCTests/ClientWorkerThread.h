#include "WorkerThread.h"
#include "ClientSocket.h"
#include <thread>
#include <mutex>

#define DEFAULT_CLIENT_MESSAGE "Hello from client"

/// <summary>
/// Worker thread for client
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
			m_socket.SendData(Message,*MessageSize);
			return;
		case READACTION:
			m_socket.WaitForData(Message,*MessageSize);
			MessageWritten = true;
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