#include <gtest/gtest.h>
#include "ClientSocket.h"
#include "ServerSocket.h"
#include "Utils.h"

/// @brief					Sends data from the client to the server
/// @param p_server			The server
/// @param p_client			The client
/// @param p_message		The message that needs to be send
/// @param p_messageLength  The length of the message
/// @param p_async			If the message should be received asynchronously
/// @param p_lateData		If the data should arrive late
/// @return					Whether the test succeeded or failed
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

/// @brief			Tests a lot of random strings that are send to the server
/// @param p_amount The amount of tests
/// @param p_server The server
/// @param p_client The client 
/// @return			Whether the test succeeded
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

/// @brief					The same as above, but sends data from server to client.
///							We use the same function here as a normal program will not run a server and a client
///							And if we wanted to use the same functions we would have to use virtual functions
/// @param p_server			The server
/// @param p_client			The client
/// @param p_message		The message that needs to be send
/// @param p_messageLength  The length of the message
/// @param p_async			If the data should be received asynchronously
/// @param p_lateData		If the data should arrive late
/// @return					Whether the test failed or succeeded
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

/// @brief			Tests a lot of random strings that are send to the client
/// @param p_amount The amount of tests
/// @param p_server The server
/// @param p_client The client
/// @return			Whether the test succeeded
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

/// @brief Connects a client and a server
#define CONNECT() \
	ServerSocket server; \
	server.ConnectAsync(); \
	ClientSocket client; \
	ASSERT_DURATION_LE(1, server.AwaitClientConnection()) \

/// @brief Tests an asynchronous connection method
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

/// @brief Tests whether a connection can be established
TEST(SocketTests, ConnectTest)
{
	CONNECT();
}

/// @brief Tests whether data can be send to a server and be received asynchronously
TEST(SocketTests, SendDataToServerTestAsync)
{
	CONNECT();
	ASSERT_DURATION_LE(1, ASSERT_TRUE(SendDataToServer(server, client, "SENDDATATOSERVER", 16, true, false)));
}

/// @brief Tests whether data can be send to a server and be received synchronously
TEST(SocketTests, SendDataToServerTestSynchronously)
{
	CONNECT();
	ASSERT_DURATION_LE(1, ASSERT_TRUE(SendDataToServer(server, client, "SENDDATATOSERVER", 16, false, false)));
}

/// @brief Tests whether data can be send to a client and be received asynchronously
TEST(SocketTests, SendDataToClientTestAsync)
{
	CONNECT();
	ASSERT_DURATION_LE(1, ASSERT_TRUE(SendDataToClient(server, client, "SENDDATATOCLIENT", 16, true, false)));
}

/// @brief Tests whether data can be send to a client and be received synchronously
TEST(SocketTests, SendDataToClientTestSychronously)
{
	CONNECT();
	ASSERT_DURATION_LE(1, ASSERT_TRUE(SendDataToClient(server, client, "SENDDATATOCLIENT", 16, false, false)));
}

/// @brief Makes sure the program throws when there is no connection to a client, but you try to send data
TEST(SocketTests, NoConnectionSendServer)
{
	ServerSocket server;
	ASSERT_FALSE(server.Connected());
	ASSERT_THROW(server.SendData("Hello", 6), std::runtime_error);
}

/// @brief Makes sure the program throws when there is no connection to a server, but you try to send data
TEST(SocketTests, NoConnectionClient)
{
	ASSERT_THROW(ClientSocket client, std::runtime_error);
}

/// @brief Makes sure the program throws when there is no connection to a client, but you try to send data
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

/// @brief Makes sure the program throws when the server is not open, but you try to send data
TEST(SocketTests, ClosedSend)
{
	CONNECT();
	ASSERT_TRUE(server.Connected());
	server.CloseServer();
	ASSERT_FALSE(server.Connected());
	ASSERT_THROW(server.SendData("Hello", 6), std::runtime_error);
}

/// @brief Tests whether a server and client can gracefully disconnect
TEST(SocketTests, DisconnectTest)
{
	CONNECT();
	server.Disconnect();
	client.Disconnect();
}

/// @brief Tests whether a server can connect to 2 clients
TEST(SocketTests, TwoClientsTest)
{
	CONNECT();
	server.Disconnect();
	client.Disconnect();
	server.ConnectAsync();
	ClientSocket client2;
	ASSERT_DURATION_LE(1, server.AwaitClientConnection());
}

/// @brief Tests if a bunch of random data can be send to the server
TEST(SocketTests, RandomSendToServerTests)
{
	CONNECT();
	ASSERT_DURATION_LE(1, ASSERT_TRUE(MultipleSendDataToServer(1000, server, client)));
}

/// @brief Tests if a bunch of random data can be send to the client
TEST(SocketTests, RandomSendToClientTests)
{
	CONNECT();
	ASSERT_DURATION_LE(1, ASSERT_TRUE(MultipleSendDataToClient(1000, server, client)));
}

/// @brief Tests if data will not be received twice
TEST(SocketTests, DontReceiveTwice)
{
	CONNECT();
	server.ReceiveDataAsync();
	client.SendData("hi", 2);
	char buffer[20];
	server.AwaitData(buffer, 20);
	ASSERT_FALSE(server.GetData(buffer, 20));
	server.ReceiveDataAsync();
	ASSERT_FALSE(server.GetData(buffer, 20));
	client.SendData("hi", 2);
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	ASSERT_TRUE(server.GetData(buffer, 20));
	ASSERT_FALSE(server.GetData(buffer, 20));
	server.ReceiveDataAsync();
	ASSERT_FALSE(server.GetData(buffer, 20));
}

/// @brief Tests if you can send a null operator
TEST(SocketTests, SendNullOp)
{
	CONNECT();
	server.ReceiveDataAsync();
	char data[4]{ "x\0x" };
	client.SendData(data,3);
	char buffer[20];
	server.AwaitData(buffer, 20);
	ASSERT_TRUE(buffer[0] == 'x' && buffer[1] == '\0' && buffer[2] == 'x');
}
