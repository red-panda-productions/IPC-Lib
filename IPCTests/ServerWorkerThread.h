#pragma once
#include "ServerSocket.h"
#include "WorkerThread.h"
#include <thread>
#include <mutex>

#define DEFAULT_SERVER_MESSAGE "Hello from server"

/// <summary>
/// Worker thread that simulates a server
/// </summary>
class ServerWorkerThread : public WorkerThread
{
private:
	ServerSocket m_socket;
public:

	ServerWorkerThread();
	
	void StartThread();

protected:
	
	void PerformAction(ACTION p_action) override;

};