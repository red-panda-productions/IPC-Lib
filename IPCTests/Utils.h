#pragma once

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