#include <gtest/gtest.h>
#include "ClientSocketAsync.h"
#include "ServerSocketAsync.h"
#include "Utils.h"

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

void GenerateRandomString(char* dataBuffer, int stringLength)
{
	for(int i = 0; i < stringLength; i++)
	{
		dataBuffer[i] = static_cast<char>(65 + rand() % 60);
	}

	dataBuffer[stringLength] = '\0';
}

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

	const int dataBufferSize = 512;
	char dataBuffer[dataBufferSize] = { '\0' };
	
	// random send tests server -> client
	for(int i = 0; i < 1000; i++)
	{
		GenerateRandomString(dataBuffer, 3 + rand() % (dataBufferSize - 4));
		bool late = rand() % 2 == 0;
		SendDataToServer(server, client, dataBuffer, strlen(dataBuffer), late);
	}

	// random send tests client -> server
	for(int i = 0; i < 1000; i++)
	{
		GenerateRandomString(dataBuffer, 3 + rand() % (dataBufferSize - 4));
		bool late = rand() % 2 == 0;
		SendDataToClient(server, client, dataBuffer, strlen(dataBuffer), late);
	}

	server.CloseServer();
	client.Disconnect();
}
