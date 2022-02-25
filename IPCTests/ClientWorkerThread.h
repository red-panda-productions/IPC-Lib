#include "WorkerThread.h"
#include "ClientSocket.h"
#include <thread>
#include <mutex>

#define DEFAULT_CLIENT_MESSAGE "Hello from client"

/// <summary>
/// Worker thread that simulates a client
/// </summary>
class ClientWorkerThread : public WorkerThread
{
private:
	ClientSocket m_socket;
public:
	ClientWorkerThread();

	void StartThread();

protected:
	void PerformAction(ACTION p_action) override;
};