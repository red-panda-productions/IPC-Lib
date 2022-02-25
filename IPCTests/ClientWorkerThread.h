#include "ClientSocket.h"
#include "TestSerializer.h"
#include <thread>
#include <mutex>

#define DEFAULT_CLIENT_MESSAGE "Hello from client"



/// <summary>
/// Woker thread for client
/// </summary>
class ClientWorkerThread
{
private:
	std::mutex mtx;
	int m_action = -1;
	ClientSocket m_socket;
	TestSerializer m_message;
	bool m_messageWritten;
public:

	/// <summary>
	/// Constructs the message that can be send
	/// </summary>
	ClientWorkerThread()
	{
		Reset();
	}

	/// <summary>
	/// Starts the worker thread
	/// </summary>
	void StartThread()
	{
		std::thread t = std::thread(&ClientWorkerThread::Loop,this);
		t.detach();
	}

	/// <summary>
	/// Sets the next action to be performed
	/// </summary>
	/// <param name="action">The action that needs to be performed</param>
	void SetAction(ACTION action)
	{
		Reset();
		mtx.lock();
		this->m_action = action;
		mtx.unlock();
	}

	void RetrieveMessage(char* message, int& size)
	{
		while (!m_messageWritten) {};
		message = m_message.Message;
		size = m_message.Size;
	}

	void Reset()
	{
		const char* m = DEFAULT_CLIENT_MESSAGE;
		m_message.Size = strlen(m);
		strcpy_s(m_message.Message, m_message.Size + 1, m);
		m_messageWritten = false;
	}

private:
	/// <summary>
	/// Loops the worker thread until stopped
	/// </summary>
	void Loop()
	{
		ACTION action = NOACTION;
		while ((action = GetAction()) != STOPACTION)
		{
			PerformAction(action);
		}
	}

	/// <summary>
	/// Performs an action
	/// </summary>
	/// <param name="action"></param>
	void PerformAction(ACTION action)
	{
		switch (action)
		{
		case WRITEACTION:
			m_socket.SendData(m_message);
			return;
		case READACTION:
			m_socket.WaitForData(m_message);
			m_messageWritten = true;
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

	/// <summary>
	/// Gets the action from the main thread
	/// </summary>
	/// <returns>The action that needs to be executed</returns>
	int GetAction()
	{
		mtx.lock();
		ACTION action = m_action;
		if (action != -1)
		{
			m_action = -1;
		}
		mtx.unlock();
		return action;
	}

};