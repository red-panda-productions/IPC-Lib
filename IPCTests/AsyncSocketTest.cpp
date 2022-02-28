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

bool MultipleSendDataToServer(int p_amount, ServerSocketAsync& p_server, ClientSocketAsync& p_client)
{
	const int dataBufferSize = 512;
	char dataBuffer[dataBufferSize] = { '\0' };
	for (int i = 0; i < p_amount; i++)
	{
		int length = 3 + rand() % (dataBufferSize - 4);
		GenerateRandomString(dataBuffer, length);
		bool late = rand() % 2 == 0;
		bool serverSend = SendDataToServer(p_server, p_client, dataBuffer, length, late);
		if (!serverSend) return false;
	}
	return true;
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

bool MultipleSendDataToClient(int p_amount, ServerSocketAsync& p_server, ClientSocketAsync& p_client)
{
	const int dataBufferSize = 512;
	char dataBuffer[dataBufferSize] = { '\0' };
	for (int i = 0; i < p_amount; i++)
	{
		int length = 3 + rand() % (dataBufferSize - 4);
		GenerateRandomString(dataBuffer, length);
		bool late = rand() % 2 == 0;
		bool clientSend = SendDataToClient(p_server, p_client, dataBuffer, length, late);
		if (!clientSend) return false;
	}
	return true;
}

#define CONNECT() \
	ServerSocketAsync server; \
	server.ConnectAsync(); \
	ClientSocketAsync client; \
	ASSERT_DURATION_LE(1, server.AwaitClientConnection()) \



TEST(AsyncSocketTests, ConnectTest)
{
	CONNECT();
}

TEST(AsyncSocketTests, SendDataToServerTest)
{
	CONNECT();

	// standard send tests
	ASSERT_DURATION_LE(1, ASSERT_TRUE(SendDataToServer(server, client, "SENDDATATOSERVER", 16, false)));
}

TEST(AsyncSocketTests, SendDataToClientTest)
{
	CONNECT();
	ASSERT_DURATION_LE(1, ASSERT_TRUE(SendDataToClient(server, client, "SENDDATATOCLIENT", 16, false)));
}

TEST(AsyncSocketTests, DisconnectTest)
{
	CONNECT();
	server.Disconnect();
	client.Disconnect();
}

TEST(AsyncSocketTests, TwoClientsTest)
{
	CONNECT();
	server.Disconnect();
	client.Disconnect();
	server.ConnectAsync();
	ClientSocketAsync client2;
	ASSERT_DURATION_LE(1, server.AwaitClientConnection());
}

TEST(AsyncSocketTests, RandomSendToServerTests)
{
	CONNECT();
	ASSERT_DURATION_LE(1, ASSERT_TRUE(MultipleSendDataToServer(1000, server, client)));
}

TEST(AsyncSocketTests, RandomSendToClientTests)
{
	CONNECT();
	ASSERT_DURATION_LE(1, ASSERT_TRUE(MultipleSendDataToClient(1000, server, client)));
}
