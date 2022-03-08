#include <gtest/gtest.h>
#include "ClientSocket.h"
#include "ServerSocket.h"
#include "Utils.h"

/// <summary>
/// Sends data from the client to the server
/// </summary>
/// <param name="p_server"> The server </param>
/// <param name="p_client"> The client </param>
/// <param name="p_message"> The message that needs to be send </param>
/// <param name="p_messageLength"> The length of the message </param>
///	<param name="p_async"> If the message should be received asynchronously </param>
/// <param name="p_lateData"> If the data should arrive late </param>
/// <returns> Whether the test succeeded or failed </returns>
bool SendDataToServer(ServerSocket& p_server, ClientSocket& p_client, const char* p_message, int p_messageLength, bool p_async, bool p_lateData)
{
	if(p_async)
	{
		p_server.ReceiveDataAsync();
	}

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
 /// Tests a lot of random strings that are send to the server
 /// </summary>
 /// <param name="p_amount"> The amount of tests </param>
 /// <param name="p_server"> The server </param>
 /// <param name="p_client"> The client </param>
 /// <returns> Whether the test succeeded </returns>
bool MultipleSendDataToServer(int p_amount, ServerSocket& p_server, ClientSocket& p_client)
{
	const int dataBufferSize = 512;
	char dataBuffer[dataBufferSize] = { '\0' };
	for (int i = 0; i < p_amount; i++)
	{
		int length = 3 + rand() % (dataBufferSize - 4);
		GenerateRandomString(dataBuffer, length);
		bool late = rand() % 2 == 0;
		bool async = rand() % 2 == 0;
		bool serverSend = SendDataToServer(p_server, p_client, dataBuffer, length, async, late);
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
/// <returns> Whether the test succeeded or failed </returns>
bool SendDataToClient(ServerSocket& p_server, ClientSocket& p_client, const char* p_message, int p_messageLength, bool p_async, bool p_lateData)
{
	if(p_async)
	{
		p_client.ReceiveDataAsync();
	}

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
 /// Tests a lot of random strings that are send to the client
 /// </summary>
 /// <param name="p_amount"> The amount of tests </param>
 /// <param name="p_server"> The server </param>
 /// <param name="p_client"> The client </param>
 /// <returns> Whether the test succeeded </returns>
bool MultipleSendDataToClient(int p_amount, ServerSocket& p_server, ClientSocket& p_client)
{
	const int dataBufferSize = 512;
	char dataBuffer[dataBufferSize] = { '\0' };
	for (int i = 0; i < p_amount; i++)
	{
		int length = 3 + rand() % (dataBufferSize - 4);
		GenerateRandomString(dataBuffer, length);
		bool late = rand() % 2 == 0;
		bool async = rand() % 2 == 0;
		bool clientSend = SendDataToClient(p_server, p_client, dataBuffer, length, async, late);
		if (!clientSend) return false;
	}
	return true;
}

/// <summary>
/// Connects a client and a server
/// </summary>
#define CONNECT() \
	ServerSocket server; \
	server.ConnectAsync(); \
	ClientSocket client; \
	ASSERT_DURATION_LE(1, server.AwaitClientConnection()) \

/// <summary>
/// Tests an asynchronous connection method
/// </summary>
TEST(SocketTests, AsyncConnectTest)
{
	ServerSocket server;
	ASSERT_FALSE(server.Connected());
	server.ConnectAsync();
	ASSERT_FALSE(server.Connected());
	ClientSocket client;
	ASSERT_DURATION_LE(1, server.AwaitClientConnection());
	ASSERT_TRUE(server.Connected());
}

/// <summary>
/// Tests whether a connection can be established
/// </summary>
TEST(SocketTests, ConnectTest)
{
	CONNECT();
}

/// <summary>
/// Tests whether data can be send to a server and be received asynchronously
/// </summary>
TEST(SocketTests, SendDataToServerTestAsync)
{
	CONNECT();
	ASSERT_DURATION_LE(1, ASSERT_TRUE(SendDataToServer(server, client, "SENDDATATOSERVER", 16, true, false)));
}

/// <summary>
/// Tests whether data can be send to a server and be received synchronously
/// </summary>
TEST(SocketTests, SendDataToServerTestSynchronously)
{
	CONNECT();
	ASSERT_DURATION_LE(1, ASSERT_TRUE(SendDataToServer(server, client, "SENDDATATOSERVER", 16, false, false)));
}
/// <summary>
/// Tests whether data can be send to a client and be received asynchronously
/// </summary>
TEST(SocketTests, SendDataToClientTestAsync)
{
	CONNECT();
	ASSERT_DURATION_LE(1, ASSERT_TRUE(SendDataToClient(server, client, "SENDDATATOCLIENT", 16, true, false)));
}

/// <summary>
/// Tests whether data can be send to a client and be received synchronously
/// </summary>
TEST(SocketTests, SendDataToClientTestSychronously)
{
	CONNECT();
	ASSERT_DURATION_LE(1, ASSERT_TRUE(SendDataToClient(server, client, "SENDDATATOCLIENT", 16, false, false)));
}

/// <summary>
/// Makes sure the program throws when there is no connection to a client, but you try to send data
/// </summary>
TEST(SocketTests, NoConnectionSendServer)
{
	ServerSocket server;
	ASSERT_FALSE(server.Connected());
	ASSERT_THROW(server.SendData("Hello", 6), std::runtime_error);
}

/// <summary>
/// Makes sure the program throws when there is no connection to a server, but you try to send data
/// </summary>
TEST(SocketTests, NoConnectionClient)
{
	ASSERT_THROW(ClientSocket client, std::runtime_error);
}

/// <summary>
/// Makes sure the program throws when there is no connection to a client, but you try to send data
/// </summary>
TEST(SocketTests, DisconnectedSend)
{
	CONNECT();
	ASSERT_TRUE(server.Connected());
	server.Disconnect();
	client.Disconnect();
	ASSERT_FALSE(server.Connected());
	ASSERT_THROW(server.SendData("Hello", 6), std::runtime_error);
	ASSERT_THROW(client.SendData("Hello", 6), std::runtime_error);
}

/// <summary>
/// Makes sure the program throws when the server is not open, but you try to send data
/// </summary>
/// <param name=""></param>
/// <param name=""></param>
TEST(SocketTests, ClosedSend)
{
	CONNECT();
	ASSERT_TRUE(server.Connected());
	server.CloseServer();
	ASSERT_FALSE(server.Connected());
	ASSERT_THROW(server.SendData("Hello", 6), std::runtime_error);
}

/// <summary>
/// Tests whether a server and client can gracefully disconnect
/// </summary>
TEST(SocketTests, DisconnectTest)
{
	CONNECT();
	server.Disconnect();
	client.Disconnect();
}

/// <summary>
/// Tests whether a server can connect to 2 clients
/// </summary>
TEST(SocketTests, TwoClientsTest)
{
	CONNECT();
	server.Disconnect();
	client.Disconnect();
	server.ConnectAsync();
	ClientSocket client2;
	ASSERT_DURATION_LE(1, server.AwaitClientConnection());
}

/// <summary>
/// Tests if a bunch of random data can be send to the server
/// </summary>
TEST(SocketTests, RandomSendToServerTests)
{
	CONNECT();
	ASSERT_DURATION_LE(1, ASSERT_TRUE(MultipleSendDataToServer(1000, server, client)));
}

/// <summary>
/// Tests if a bunch of random data can be send to the client
/// </summary>
TEST(SocketTests, RandomSendToClientTests)
{
	CONNECT();
	ASSERT_DURATION_LE(1, ASSERT_TRUE(MultipleSendDataToClient(1000, server, client)));
}
