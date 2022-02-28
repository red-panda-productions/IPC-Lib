#pragma once
#include <gtest/gtest.h>
#include <future>
#include <csetjmp>
#include <csignal>

/// <summary>
/// Executes a statement, and fails if it takes too long
/// </summary>
/// <param name="secs"> The timeout parameter </param>
/// <param name="stmt"> The statement to be executed </param>
#define ASSERT_DURATION_LE(secs, stmt) { \
  std::promise<bool> completed; \
  auto stmt_future = completed.get_future(); \
  std::thread([&](std::promise<bool>& completed) { \
    stmt; \
    completed.set_value(true); \
  }, std::ref(completed)).detach(); \
  if(stmt_future.wait_for(std::chrono::seconds(secs)) == std::future_status::timeout) \
    GTEST_FATAL_FAILURE_("       timed out (> " #secs \
    " seconds). Check code for infinite loops"); \
}

/// <summary>
/// Tests if 2 messages are equal to each other
/// </summary>
/// <param name="msg1"> The first message </param>
/// <param name="msg2"> The second message </param>
/// <param name="size"> The size of the 2 messages </param>
/// <returns> Whether the test failed or succeeded </return>
inline bool TestMessageEqual(const char* msg1, const char* msg2, int size)
{
	bool len1 = size == strlen(msg1);
	if (!len1) return false;
	int length = strlen(msg2);
	bool len2 = size == length;
	if (!len2) return false;

	for(int i = 0; i < size; i++)
	{
		if (msg1[i] != msg2[i]) return false;
	}
	return true;
}

/// <summary>
/// Generates a random string for testing
/// </summary>
/// <param name="dataBuffer"> The buffer in which the string will be written </param>
/// <param name="stringLength"> The length of the string (must be strictly smaller than the length of the buffer) </param>
inline void GenerateRandomString(char* dataBuffer, int stringLength)
{
	for (int i = 0; i < stringLength; i++)
	{
		dataBuffer[i] = static_cast<char>(65 + rand() % 60);
	}
	dataBuffer[stringLength] = '\0';
}