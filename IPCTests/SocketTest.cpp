#include <gtest/gtest.h>
#include "ClientSocket.h"
#include "ServerSocket.h"
#include "Utils.h"

#define AWAIT_MESSAGE_TIMEOUT    2
#define AWAIT_CONNECTION_TIMEOUT 3

#ifdef WIN32
#define DOUBLE_INIT_CODE IPCLIB_SERVER_ERROR
#endif
#ifdef __linux__
#define DOUBLE_INIT_CODE IPCLIB_SUCCEED
#endif
#define AMOUNT_OF_TESTS 200

/// @brief                 Sends data from the client to the server
/// @param p_server        The server
/// @param p_client        The client
/// @param p_message       The message that needs to be send
/// @param p_messageLength The length of the message
/// @param p_async         If the message should be received asynchronously
/// @param p_lateData  	   If the data should arrive late
/// @return                Whether the test succeeded or failed
bool SendDataToServer(ServerSocket& p_server, ClientSocket& p_client, const char* p_message, int p_messageLength, bool p_async, bool p_lateData)
{
    if (p_async)
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

    if (p_client.SendData(p_message, p_messageLength) != IPCLIB_SUCCEED)
    {
        return false;
    }

    if (p_server.AwaitData(dataBuffer, dataBufferSize) != IPCLIB_SUCCEED)
    {
        return false;
    }

    return TestMessageEqual(p_message, dataBuffer, p_messageLength);
}

/// @brief          Tests a lot of random strings that are send to the server
/// @param p_amount The amount of tests
/// @param p_server The server
/// @param p_client The client
/// @return         Whether the test succeeded
bool MultipleSendDataToServer(int p_amount, ServerSocket& p_server, ClientSocket& p_client)
{
    const int dataBufferSize = 512;
    char dataBuffer[dataBufferSize] = {'\0'};
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

/// @brief                  The same as above, but sends data from server to client.
///                         We use the same function here as a normal program will not run a server and a client
///                         And if we wanted to use the same functions we would have to use virtual functions
/// @param p_server         The server
/// @param p_client         The client
/// @param p_message        The message that needs to be send
/// @param p_messageLength  The length of the message
/// @param p_async          If the data should be received asynchronously
/// @param p_lateData       If the data should arrive late
/// @return                 Whether the test failed or succeeded
bool SendDataToClient(ServerSocket& p_server, ClientSocket& p_client, const char* p_message, int p_messageLength, bool p_async, bool p_lateData)
{
    if (p_async)
    {
        p_client.ReceiveDataAsync();
    }

    char dataBuffer[512];
    int dataBufferSize = 512;

    if (p_lateData)
    {
        bool received = p_client.GetData(dataBuffer, dataBufferSize);
        if (received) return false;
    }

    if (p_server.SendData(p_message, p_messageLength) != IPCLIB_SUCCEED)
    {
        return false;
    }

    if (p_client.AwaitData(dataBuffer, dataBufferSize) != IPCLIB_SUCCEED)
    {
        return false;
    }

    return TestMessageEqual(p_message, dataBuffer, p_messageLength);
}

/// @brief          Tests a lot of random strings that are send to the client
/// @param p_amount The amount of tests
/// @param p_server The server
/// @param p_client The client
/// @return         Whether the test succeeded
bool MultipleSendDataToClient(int p_amount, ServerSocket& p_server, ClientSocket& p_client)
{
    const int dataBufferSize = 512;
    char dataBuffer[dataBufferSize] = {'\0'};
    for (int i = 0; i < p_amount; i++)
    {
        int length = 3 + rand() % (dataBufferSize - 4);
        GenerateRandomString(dataBuffer, length);
        bool late = rand() % 2 == 0;
        bool async = rand() % 2 == 0;
        bool clientSend = SendDataToClient(p_server, p_client, dataBuffer, length, async, late);
        if (!clientSend)
        {
            std::cout << i << std::endl;
            return false;
        }
    }
    return true;
}

/// @brief Connects a client and a server
#define CONNECT()                                   \
    ServerSocket server;                            \
    ASSERT_EQ(server.Initialize(), IPCLIB_SUCCEED); \
    server.ConnectAsync();                          \
    ClientSocket client;                            \
    ASSERT_EQ(client.Initialize(), IPCLIB_SUCCEED); \
    ASSERT_DURATION_LE(AWAIT_CONNECTION_TIMEOUT, server.AwaitClientConnection())

/// @brief Tests an asynchronous connection method
TEST(SocketTests, AsyncConnectTest)
{
    ServerSocket server;
    ASSERT_EQ(server.Initialize(), IPCLIB_SUCCEED);
    ASSERT_FALSE(server.Connected());
    server.ConnectAsync();
    ASSERT_FALSE(server.Connected());
    ClientSocket client;
    ASSERT_EQ(client.Initialize(), IPCLIB_SUCCEED);
    ASSERT_DURATION_LE(AWAIT_MESSAGE_TIMEOUT, server.AwaitClientConnection());
    ASSERT_TRUE(server.Connected());
}

/// @brief Tests whether a connection can be established
TEST(SocketTests, ConnectTest)
{
    CONNECT();
}

void AwaitingServer()
{
    ServerSocket server;
    ASSERT_EQ(server.Initialize(), IPCLIB_SUCCEED);
    ASSERT_EQ(server.AwaitClientConnection(), IPCLIB_SUCCEED);
    char buffer[32];
    ASSERT_EQ(server.AwaitData(buffer, 32), IPCLIB_SUCCEED);
    ASSERT_EQ(server.SendData("OK", 2), IPCLIB_SUCCEED);
    TestMessageEqual("OK", buffer, 2);
}

TEST(SocketTests, AwaitConnectionTest)
{
    std::thread t(AwaitingServer);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ClientSocket client;
    ASSERT_EQ(client.Initialize(), IPCLIB_SUCCEED);
    client.ReceiveDataAsync();
    ASSERT_EQ(client.SendData("OK", 2), IPCLIB_SUCCEED);
    char buffer[32];
    ASSERT_DURATION_LE(AWAIT_MESSAGE_TIMEOUT, ASSERT_EQ(client.AwaitData(buffer, 32), IPCLIB_SUCCEED));
    TestMessageEqual("OK", buffer, 2);
    t.join();
}

/// @brief Tests whether data can be send to a server and be received asynchronously
TEST(SocketTests, SendDataToServerTestAsync)
{
    CONNECT();
    ASSERT_DURATION_LE(AWAIT_MESSAGE_TIMEOUT, ASSERT_TRUE(SendDataToServer(server, client, "SENDDATATOSERVER", 16, true, false)));
}

/// @brief Tests whether data can be send to a server and be received synchronously
TEST(SocketTests, SendDataToServerTestSynchronously)
{
    CONNECT();
    ASSERT_DURATION_LE(AWAIT_MESSAGE_TIMEOUT, ASSERT_TRUE(SendDataToServer(server, client, "SENDDATATOSERVER", 16, false, false)));
}

/// @brief Tests whether data can be send to a client and be received asynchronously
TEST(SocketTests, SendDataToClientTestAsync)
{
    CONNECT();
    ASSERT_DURATION_LE(AWAIT_MESSAGE_TIMEOUT, ASSERT_TRUE(SendDataToClient(server, client, "SENDDATATOCLIENT", 16, true, false)));
}

/// @brief Tests whether data can be send to a client and be received synchronously
TEST(SocketTests, SendDataToClientTestSychronously)
{
    CONNECT();
    ASSERT_DURATION_LE(AWAIT_MESSAGE_TIMEOUT, ASSERT_TRUE(SendDataToClient(server, client, "SENDDATATOCLIENT", 16, false, false)));
}

/// @brief Makes sure the program throws when there is no connection to a client, but you try to send data
TEST(SocketTests, NoConnectionSendServer)
{
    ServerSocket server;
    ASSERT_EQ(server.Initialize(), IPCLIB_SUCCEED);
    ASSERT_FALSE(server.Connected());
    ASSERT_EQ(server.SendData("Hello", 6), IPCLIB_SERVER_ERROR);
}

/// @brief Makes sure the program throws when there is no connection to a server, but you try to send data
TEST(SocketTests, NoConnectionClient)
{
    ClientSocket client;
    ASSERT_EQ(client.Initialize(), SOCKET_LIBRARY_ERROR);
}

/// @brief Makes sure the program throws when there is no connection to a client, but you try to send data
TEST(SocketTests, DisconnectedSend)
{
    CONNECT();
    ASSERT_TRUE(server.Connected());
    ASSERT_EQ(server.Disconnect(), IPCLIB_SUCCEED);
    ASSERT_EQ(client.Disconnect(), IPCLIB_SUCCEED);
    ASSERT_FALSE(server.Connected());
    ASSERT_EQ(server.SendData("Hello", 6), IPCLIB_SERVER_ERROR);
    ASSERT_EQ(client.SendData("Hello", 6), IPCLIB_CLOSED_CONNECTION_ERROR);
}

/// @brief Makes sure the program throws when the server is not open, but you try to send data
TEST(SocketTests, ClosedSend)
{
    CONNECT();
    ASSERT_TRUE(server.Connected());
    ASSERT_EQ(server.Disconnect(), IPCLIB_SUCCEED);
    ASSERT_FALSE(server.Connected());
    ASSERT_EQ(server.SendData("Hello", 6), IPCLIB_SERVER_ERROR);
    ASSERT_EQ(server.CloseServer(), IPCLIB_SUCCEED);
    ASSERT_FALSE(server.Connected());
    ASSERT_EQ(server.SendData("Hello", 6), IPCLIB_CLOSED_CONNECTION_ERROR);
}

/// @brief Tests whether a server and client can gracefully disconnect
TEST(SocketTests, DisconnectTest)
{
    CONNECT();
    ASSERT_EQ(server.Disconnect(), IPCLIB_SUCCEED);
    ASSERT_EQ(client.Disconnect(), IPCLIB_SUCCEED);
}

/// @brief Tests whether a server can connect to 2 clients
TEST(SocketTests, TwoClientsTest)
{
    CONNECT();
    ASSERT_EQ(server.Disconnect(), IPCLIB_SUCCEED);
    ASSERT_EQ(client.Disconnect(), IPCLIB_SUCCEED);
    server.ConnectAsync();
    ClientSocket client2;
    ASSERT_EQ(client2.Initialize(), IPCLIB_SUCCEED);
    ASSERT_DURATION_LE(AWAIT_CONNECTION_TIMEOUT, server.AwaitClientConnection());
}

/// @brief Tests if a bunch of random data can be send to the server
TEST(SocketTests, RandomSendToServerTests)
{
    CONNECT();
    ASSERT_DURATION_LE(AWAIT_MESSAGE_TIMEOUT, ASSERT_TRUE(MultipleSendDataToServer(1000, server, client)));
}

/// @brief Tests if a bunch of random data can be send to the client
TEST(SocketTests, RandomSendToClientTests)
{
    CONNECT();
    ASSERT_DURATION_LE(AWAIT_MESSAGE_TIMEOUT, ASSERT_TRUE(MultipleSendDataToClient(1000, server, client)));
}

/// @brief Tests if data will not be received twice
TEST(SocketTests, DontReceiveTwice)
{
    CONNECT();
    server.ReceiveDataAsync();
    ASSERT_EQ(client.SendData("hi1", 3), IPCLIB_SUCCEED);
    char buffer[20];
    ASSERT_DURATION_LE(AWAIT_MESSAGE_TIMEOUT, ASSERT_EQ(server.AwaitData(buffer, 20), IPCLIB_SUCCEED));
    ASSERT_TRUE(TestMessageEqual(buffer, "hi1", 3));
    ASSERT_FALSE(server.GetData(buffer, 20));
    server.ReceiveDataAsync();
    ASSERT_FALSE(server.GetData(buffer, 20));
    ASSERT_EQ(client.SendData("hi2", 3), IPCLIB_SUCCEED);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_TRUE(server.GetData(buffer, 20));
    ASSERT_TRUE(TestMessageEqual(buffer, "hi2", 3));
    ASSERT_FALSE(server.GetData(buffer, 20));
    server.ReceiveDataAsync();
    ASSERT_FALSE(server.GetData(buffer, 20));
    ASSERT_EQ(client.SendData("hi3", 3), IPCLIB_SUCCEED);
    ASSERT_DURATION_LE(AWAIT_MESSAGE_TIMEOUT, ASSERT_EQ(server.AwaitData(buffer, 20), IPCLIB_SUCCEED));
    ASSERT_TRUE(TestMessageEqual(buffer, "hi3", 3));
}

/// @brief Tests if you can send a null operator
TEST(SocketTests, SendNullOp)
{
    CONNECT();
    server.ReceiveDataAsync();
    char data[4]{"x\0x"};
    ASSERT_EQ(client.SendData(data, 3), IPCLIB_SUCCEED);
    char buffer[20];
    ASSERT_DURATION_LE(AWAIT_MESSAGE_TIMEOUT, ASSERT_EQ(server.AwaitData(buffer, 20), IPCLIB_SUCCEED));
    ASSERT_TRUE(buffer[0] == 'x' && buffer[1] == '\0' && buffer[2] == 'x');
}

/// @brief Tests if the server crashes when 2 servers are on the same ip and port
TEST(SocketTests, DoubleServer)
{
    ServerSocket server1;
    ASSERT_EQ(server1.Initialize(), IPCLIB_SUCCEED);
    ServerSocket server2;
    ASSERT_EQ(server2.Initialize(), DOUBLE_INIT_CODE);
}

/// @brief Tests if the client crashes when there is no server
TEST(SocketTests, NoServer)
{
    ClientSocket client1;
    ASSERT_EQ(client1.Initialize(), SOCKET_LIBRARY_ERROR);
}

/// @brief Tests if the destructor is called correctly when the worker thread is still reading
TEST(SocketTests, DeleteAfterReceiveAsync)
{
    CONNECT();
    server.ReceiveDataAsync();
    ASSERT_DURATION_LE(AWAIT_CONNECTION_TIMEOUT, server.~ServerSocket());
}

/// @brief Tests if an exception is thrown when initialize is called twice
TEST(SocketTests, DoubleServerInitializeTest)
{
    ServerSocket server;
    ASSERT_EQ(server.Initialize(), IPCLIB_SUCCEED);
    ASSERT_EQ(server.Initialize(), IPCLIB_SUCCEED);  // should not throw
}

/// @brief Tests if the correct error code is returned when initialize is called twice
TEST(SocketTests, DoubleClientInitializeTest)
{
    CONNECT();
    ASSERT_EQ(client.Initialize(), IPCLIB_SUCCEED);
}

/// @brief should not throw when initializing twice
TEST(SocketTests, ClientFailedInitializeTwiceTest)
{
    ClientSocket client;
    ASSERT_EQ(client.Initialize(), SOCKET_LIBRARY_ERROR);
    ASSERT_EQ(client.Initialize(), SOCKET_LIBRARY_ERROR);  // should not throw
}

/// @brief Checks if an error code can be received from the receiving thread
TEST(SocketTests, ReceivingThreadThrow)
{
    CONNECT();
    char buffer[20];
    client.ReceiveDataAsync();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    server.~ServerSocket();
    ASSERT_EQ(client.AwaitData(buffer, 20), IPCLIB_RECEIVE_ERROR);
}

/// @brief The thread side of the exhaustion test
void ExhaustionClientThreadSide()
{
    ClientSocket client;
    client.Initialize();

    char buffer[20];
    for (int i = 0; i < AMOUNT_OF_TESTS; i++)
    {
        ASSERT_DURATION_LE(2, client.AwaitData(buffer, 20));
        ASSERT_DURATION_LE(2, client.ReceiveDataAsync());
        ASSERT_EQ(client.SendData("Hi", 3), IPCLIB_SUCCEED);
    }
}

/// @brief Tests whether either side (client or server) can get exhausted by sending rapid messages
TEST(SocketTests, ExhaustionTest)
{
    ServerSocket server;
    server.Initialize();
    server.ConnectAsync();
    std::thread t(ExhaustionClientThreadSide);

    server.AwaitClientConnection();
    ASSERT_DURATION_LE(2, server.ReceiveDataAsync());

    char buffer[20];
    for (int i = 0; i < AMOUNT_OF_TESTS; i++)
    {
        ASSERT_EQ(server.SendData("Hello", 6), IPCLIB_SUCCEED);
        ASSERT_DURATION_LE(2, server.AwaitData(buffer, 20));
        ASSERT_DURATION_LE(2, server.ReceiveDataAsync());
    }

    t.join();
}