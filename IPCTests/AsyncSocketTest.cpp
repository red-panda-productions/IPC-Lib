#include <gtest/gtest.h>
#include "ClientSocketAsync.h"
#include "ServerSocketAsync.h"
#include "Utils.h"

/// <summary>
/// Sends data from the client to the server
/// </summary>
/// <param name="p_server"> The server </param>
/// <param name="p_client"> The client </param>
/// <param name="p_message"> The message that needs to be send </param>
/// <param name="p_messageLength"> The length of the message </param>
/// <param name="p_lateData"> If the data should arrive late </param>
/// <returns> Whether the test succeeded or failed </return>
bool SendDataToServer(ServerSocketAsync& p_server, ClientSocketAsync& p_client, const char* p_message, int p_messageLength, bool p_lateData)
{
	p_server.ReceiveDataAsync();

	char dataBuffer[512];
	int dataBufferSize = 512;

	if (p_lateData)
	{
		bool received = p_server.GetData(dataBuffer, dataBufferSize);
		if (received) return false;
	}

	p_client.SendData(p_message, p_messageLength);

	p_server.AwaitData(dataBuffer, dataBufferSize);

	return TestMessageEqual(p_message, dataBuffer, p_messageLength);
}

/// <summary>
/// The same as above, but sends data from server to client.
///	We use the same function here as a normal program will not run a server and a client
///	And if we wanted to use the same functions we would have to use virtual functions
/// </summary>
/// <param name="p_server"> The server </param>
/// <param name="p_client"> The client </param>
/// <param name="p_message"> The message that needs to be send </param>
/// <param name="p_messageLength"> The length of the message </param>
/// <param name="p_lateData"> If the data should arrive late </param>
/// <returns> Whether the test succeeded or failed </return>
bool SendDataToClient(ServerSocketAsync& p_server, ClientSocketAsync& p_client, const char* p_message, int p_messageLength, bool p_lateData)
{
	p_client.ReceiveDataAsync();

	char dataBuffer[512];
	int dataBufferSize = 512;

	if(p_lateData)
	{
		bool received = p_client.GetData(dataBuffer, dataBufferSize);
		if (received) return false;
	}

	p_server.SendData(p_message, p_messageLength);


	p_client.AwaitData(dataBuffer, dataBufferSize);

	return TestMessageEqual(p_message, dataBuffer, p_messageLength);
}


/// <summary>
/// Tests a complete asynchronous socket system
/// </summary>
TEST(CompleteAsyncSocketTest, AsyncSocketTests)
{
	ServerSocketAsync server;
	server.ConnectAsync();

	ClientSocketAsync client;
	server.AwaitClientConnection();

	// standard send tests
	bool serverSend = SendDataToServer(server, client, "SENDDATATOSERVER", 16, false);
	ASSERT_TRUE(serverSend);
	bool clientSend = SendDataToClient(server, client, "SENDDATATOCLIENT", 16, false);
	ASSERT_TRUE(clientSend);

	// standard send tests with waiting on data
	serverSend = SendDataToServer(server, client, "SENDDATATOSERVER", 16, true);
	ASSERT_TRUE(serverSend);
	clientSend = SendDataToClient(server, client, "SENDDATATOCLIENT", 16, true);
	ASSERT_TRUE(clientSend);

	// client disconnect test
	server.Disconnect();
	client.Disconnect();

	server.ConnectAsync();
	ClientSocketAsync client2;
	server.AwaitClientConnection();

	const int dataBufferSize = 512;
	char dataBuffer[dataBufferSize] = { '\0' };
	
	// random send tests server -> client
	for(int i = 0; i < 1000; i++)
	{
		int length = 3 + rand() % (dataBufferSize - 4);
		GenerateRandomString(dataBuffer, length);
		bool late = rand() % 2 == 0;
		serverSend = SendDataToServer(server, client2, dataBuffer, length , late);
		ASSERT_TRUE(serverSend);
	}

	// random send tests client -> server
	for(int i = 0; i < 1000; i++)
	{
		int length = 3 + rand() % (dataBufferSize - 4);
		GenerateRandomString(dataBuffer, length);
		bool late = rand() % 2 == 0;
		clientSend = SendDataToClient(server, client2, dataBuffer, length, late);
		ASSERT_TRUE(clientSend);
	}

	server.CloseServer();
	client2.Disconnect();
}
