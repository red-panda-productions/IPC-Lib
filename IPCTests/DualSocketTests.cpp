
#include <gtest/gtest.h>
#include "Actions.h"
#include "ClientWorkerThread.h"
#include "ServerWorkerThread.h"
#include <thread>
#include <chrono>

/// <summary>
/// Test sending data from server to client
/// </summary>
/// <param name="p_swt">server</param>
/// <param name="p_cwt">client</param>
/// <returns></returns>
bool TestServerSend(ServerWorkerThread& p_swt, ClientWorkerThread& p_cwt)
{
	p_cwt.SetAction(ACTION_READ);
	p_swt.SetAction(ACTION_WRITE);
	char message[256];
	int size = 256;
	p_cwt.RetrieveMessage(message, size);
	const char* expected = DEFAULT_SERVER_MESSAGE;
	if (size != strlen(expected)) return false;
	for(int i = 0; i < size; i++)
	{
		if (message[i] == expected[i]) continue;
		return false;
	}
	return true;
}

/// <summary>
/// Tests sending data from client to server
/// </summary>
/// <param name="p_swt">server</param>
/// <param name="p_cwt">client</param>
/// <returns></returns>
bool TestClientSend(ServerWorkerThread& p_swt, ClientWorkerThread& p_cwt)
{
	p_cwt.SetAction(ACTION_WRITE);
	p_swt.SetAction(ACTION_READ);
	char message[256];
	int size = 256;
	p_swt.RetrieveMessage(message, size);
	const char* expected = DEFAULT_CLIENT_MESSAGE;
	if (size != strlen(expected)) return false;
	for(int i = 0; i < size; i++)
	{
		if (message[i] == expected[i]) continue;
		return false;
	}
	return true;
}

/// <summary>
/// Tests sending data between client and server
/// </summary>
TEST(CompleteSocketTest, ExampleSocketTest)
{
	ServerWorkerThread swt;
    ClientWorkerThread cwt;

	swt.StartThread();
	swt.SetAction(ACTION_CONNECT);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	cwt.StartThread();

	bool result = TestServerSend(swt, cwt);
	swt.Reset();
	cwt.Reset();
	ASSERT_TRUE(result) << "TEST SEVER SEND FAILED";

	result = TestClientSend(swt, cwt);
	swt.Reset();
	cwt.Reset();
	ASSERT_TRUE(result) << "TEST CLIENT SEND FAILED";

	swt.SetAction(ACTION_DISCONNECT);
	cwt.SetAction(ACTION_DISCONNECT);

	swt.SetAction(ACTION_CLOSE_SERVER);

	swt.SetAction(ACTION_DECONSTRUCT);
    cwt.SetAction(ACTION_DECONSTRUCT);

	swt.SetAction(ACTION_STOP);
	cwt.SetAction(ACTION_STOP);
}
