#include <gtest/gtest.h>
#include "Actions.h"
#include "ClientWorkerThread.h"
#include "ServerWorkerThread.h"
#include <thread>
#include <chrono>

/// <summary>
/// Test sending data from one WorkerThread to another
/// </summary>
/// <param name="p_from">sending WorkerThread</param>
/// <param name="p_to">receiving WorkerThread</param>
/// <returns></returns>
bool SendMessageTest(WorkerThread& p_from, WorkerThread& p_to, const char* p_expectedMessage)
{
    p_from.SetAction(ACTION_WRITE);
    p_to.SetAction(ACTION_READ);
    char message[256];
    int size = 256;
    p_to.RetrieveMessage(message, size);
    if (size != strlen(p_expectedMessage)) return false;
    for(int i = 0; i < size; i++)
    {
        if (message[i] == p_expectedMessage[i]) continue;
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

    bool result = SendMessageTest(swt, cwt, DEFAULT_SERVER_MESSAGE);
    swt.Reset();
    cwt.Reset();
    ASSERT_TRUE(result) << "TEST SERVER SEND FAILED";

    result = SendMessageTest(cwt, swt, DEFAULT_CLIENT_MESSAGE);
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
