
#include <gtest/gtest.h>
#include "Actions.h"
#include "ClientWorkerThread.h"
#include "ServerWorkerThread.h"
#include <thread>
#include <chrono>

bool TestServerSend(ServerWorkerThread& swt, ClientWorkerThread& cwt)
{
	cwt.SetAction(READACTION);
	swt.SetAction(WRITEACTION);
	char message[256];
	int size = 0;
	cwt.RetrieveMessage(message, size);
	const char* expected = DEFAULT_SERVER_MESSAGE;
	if (size != strlen(expected)) return false;
	for(int i = 0; i < size; i++)
	{
		if (message[i] == expected[i]) continue;
		return false;
	}
	return true;
}

bool TestClientSend(ServerWorkerThread& swt, ClientWorkerThread& cwt)
{
	cwt.SetAction(WRITEACTION);
	swt.SetAction(READACTION);
	char message[256];
	int size = 0;
	swt.RetrieveMessage(message, size);
	const char* expected = DEFAULT_CLIENT_MESSAGE;
	if (size != strlen(expected)) return false;
	for(int i = 0; i < size; i++)
	{
		if (message[i] == expected[i]) continue;
		return false;
	}
	return true;
}

TEST(CompleteSocketTest, ExampleSocketTest)
{
	ServerWorkerThread swt;
    ClientWorkerThread cwt;

	swt.StartThread();
	swt.SetAction(CONNECTACTION);
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

	swt.SetAction(DISCONNECTACTION);
	cwt.SetAction(DISCONNECTACTION);

	swt.SetAction(CLOSESERVERACTION);

	swt.SetAction(DECONSTRUCTACTION);
    cwt.SetAction(DECONSTRUCTACTION);

	swt.SetAction(STOPACTION);
	cwt.SetAction(STOPACTION);
}
