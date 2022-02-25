#include "ServerSocket.h"
#include "TestSerializer.h"
#include <thread>
#include <mutex>

#define DEFAULT_SERVER_MESSAGE "Hello from server"

/// <summary>
/// Woker thread for client
/// </summary>
class ServerWorkerThread
{
private:
	std::mutex mtx;
	int m_action = -1;
	ServerSocket m_socket;
	TestSerializer m_message;
	bool m_messageWritten;
public:
	
	/// <summary>
	/// Constructs the message that can be send
	/// </summary>
	ServerWorkerThread()
	{
		Reset();
	}

	/// <summary>
	/// Starts the worker thread
	/// </summary>
	void StartThread()
	{
		std::thread t = std::thread(&ServerWorkerThread::Loop, this);
		t.detach();
	}

	/// <summary>
	/// Sets the next action to be performed
	/// </summary>
	/// <param name="action">The action that needs to be performed</param>
	void SetAction(ACTION action)
	{
		mtx.lock();
		this->m_action = action;
		mtx.unlock();
	}

	/// <summary>
	/// Sets the message that has to be send
	/// </summary>
	/// <param name="message"></param>
	void SetMessage(const char* message)
	{
		this->m_message.Size = strlen(message);
		strcpy_s(this->m_message.Message, this->m_message.Size + 1, message);
	}

	void RetrieveMessage(char* message, int& size)
	{
		while (!m_messageWritten) {};
		message = m_message.Message;
		size = m_message.Size;
	}


	void Reset()
	{
		m_messageWritten = false;
		const char* m = DEFAULT_SERVER_MESSAGE;
		m_message.Size = strlen(m);
		strcpy_s(m_message.Message, m_message.Size + 1, m);
	}

private:
	/// <summary>
	/// Loops the worker thread until stopped
	/// </summary>
	void Loop()
	{
		m_socket.WaitForClientConnection();
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
		case CLOSESERVERACTION:
			m_socket.CloseServer();
			return;
		case DECONSTRUCTACTION:
			m_socket.~ServerSocket();
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