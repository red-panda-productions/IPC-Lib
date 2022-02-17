#include "ServerSocket.h"
#include "TestSerializer.h"
#include <thread>
#include <mutex>


/// <summary>
/// Woker thread for client
/// </summary>
class ServerWorkerThread
{
private:
	std::mutex mtx;
	int m_action = -1;
	ServerSocket m_socket;
public:
	TestSerializer Message;

	/// <summary>
	/// Constructs the message that can be send
	/// </summary>
	ServerWorkerThread()
	{
		const char* m = "Hello from server";
		Message.m_size = strlen(m);
		strcpy_s(Message.m_message, Message.m_size, m);
	}

	/// <summary>
	/// Starts the worker thread
	/// </summary>
	void StartThread()
	{
		std::thread(Loop);
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
		strcpy(this->Message.m_message, message);
		this->Message.m_size = strlen(message);
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
			m_socket.SendData(Message);
			return;
		case READACTION:
			m_socket.WaitForData(Message);
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