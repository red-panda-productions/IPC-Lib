
#include <gtest/gtest.h>
#include "Actions.h"
#include "ClientWorkerThread.h"
#include "ServerWorkerThread.h"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
	// Expect two strings not to be equal.
	EXPECT_STRNE("hello", "world");
	// Expect equality.
	EXPECT_EQ(7 * 6, 42);
}

TEST(SocketTest, ExampleSocketTest)
{
	ServerWorkerThread swt;
    ClientWorkerThread cwt;
	swt.SetAction(DECONSTRUCTACTION);
    cwt.SetAction(DECONSTRUCTACTION);
}
