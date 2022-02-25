#include "ServerWorkerThread.h"

ServerWorkerThread::ServerWorkerThread() : WorkerThread(DEFAULT_SERVER_MESSAGE)
{
}

/// <summary>
/// Starts the worker thread
/// </summary>
void ServerWorkerThread::StartThread()
{
	std::thread t = std::thread(&ServerWorkerThread::Loop, this);
	t.detach();
}

/// <summary>
/// Performs an action
/// </summary>
/// <param name="p_action"></param>
void ServerWorkerThread::PerformAction(ACTION p_action)
{
	switch (p_action)
	{
	case ACTION_WRITE:
		m_socket.SendData(Message, *MessageSize);
		return;
	case ACTION_READ:
		m_socket.WaitForData(Message, *MessageSize);
		MessageWritten = true;
		return;
	case ACTION_DISCONNECT:
		m_socket.Disconnect();
		return;
	case ACTION_CLOSE_SERVER:
		m_socket.CloseServer();
		return;
	case ACTION_DECONSTRUCT:
		m_socket.~ServerSocket();
		return;
	case ACTION_CONNECT:
		m_socket.WaitForClientConnection();
		return;
	default:
		return;
	}
}