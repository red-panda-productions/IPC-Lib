#include "ClientWorkerThread.h"

ClientWorkerThread::ClientWorkerThread() : WorkerThread(DEFAULT_CLIENT_MESSAGE)
{
}

/// <summary>
/// Starts the worker thread
/// </summary>
void ClientWorkerThread::StartThread()
{
	std::thread t = std::thread(&ClientWorkerThread::Loop, this);
	t.detach();
}

/// <summary>
/// Performs an action
/// </summary>
/// <param name="p_action"></param>
void ClientWorkerThread::PerformAction(ACTION p_action)
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
	case ACTION_DECONSTRUCT:
		m_socket.~ClientSocket();
		return;
	default:
		return;
	}
}