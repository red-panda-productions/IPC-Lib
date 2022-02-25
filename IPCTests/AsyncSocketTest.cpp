#include <gtest/gtest.h>
#include "ClientSocketAsync.h"
#include "ServerSocketAsync.h"


TEST(CompleteAsyncSocketTest, AsyncSocketTests)
{
	ServerSocketAsync server;
	server.ConnectAsync();

	ClientSocketAsync client;
	server.AwaitClientConnection();


}