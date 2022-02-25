#pragma once
#include "Actions.h"
#include <thread>
#include <mutex>

/// <summary>
/// A worker thread that can simulate a server or a client
/// </summary>
class WorkerThread
{
protected:
	std::mutex* Action_lock = new std::mutex();
	ACTION* Action = new ACTION(-1);
	bool MessageWritten;
	const char* defaultMessage;
	char* Message = nullptr;
	int* MessageSize = new int(-1);

	int GetAction();

	void Loop();

	virtual void PerformAction(ACTION p_action) = 0;

public:
	WorkerThread(const char* p_defaultMessage);
	
	void Reset();

	void RetrieveMessage(char* p_messageBuffer, int& p_size);

	void SetAction(ACTION action);
};