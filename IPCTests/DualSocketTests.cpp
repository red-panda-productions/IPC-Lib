
#include <gtest/gtest.h>
#include "Actions.h"
#include "ClientWorkerThread.h"
#include "ServerWorkerThread.h"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
	// Expect two strings not to be equal.
	EXPECT_STRNE("hello", "world");
	// Expect equality.
	EXPECT_EQ(7 * 5, 42);
}

TEST(SocketTest, ExampleSocketTest)
{
	ClientWorkerThread cwt;
	ServerWorkerThread swt;
	cwt.SetAction(DECONSTRUCTACTION);
	swt.SetAction(DECONSTRUCTACTION);
}
