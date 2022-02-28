#pragma once
#include <string>

bool TestMessageEqual(const char* msg1, const char* msg2, int size)
{
	int length = strlen(msg1);
	bool len1 = size == length;
	if (!len1) return false;
	bool len2 = size == strlen(msg2);
	if (!len2) return false;

	for(int i = 0; i < size; i++)
	{
		if (msg1[i] != msg2[i]) return false;
	}
	return true;
}