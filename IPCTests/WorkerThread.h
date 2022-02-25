#pragma once
#include "TestSerializer.h"
#include <thread>
#include <mutex>
class WorkerThread
{
protected:
	std::mutex* Action_lock = new std::mutex();
	std::mutex* Message_lock = new std::mutex();
	ACTION* Action = new ACTION(-1);
	TestSerializer* Message = new TestSerializer();
	bool MessageWritten;
	const char* defaultMessage;

	/// <summary>
	/// Gets the action from the main thread
	/// </summary>
	/// <returns>The action that needs to be executed</returns>
	int GetAction()
	{
		Action_lock->lock();
		ACTION action = *Action;
		if (action != NOACTION)
		{
			*Action = NOACTION;
		}
		Action_lock->unlock();
		return action;
	}

	/// <summary>
	/// Loops the worker thread until stopped
	/// </summary>
	void Loop()
	{
		ACTION action = NOACTION;
		while ((action = GetAction()) != STOPACTION)
		{
			PerformAction(action);
		}
	}

	virtual void PerformAction(ACTION action) = 0;

public:
	WorkerThread(const char* defaultMessage)
	{
		this->defaultMessage = defaultMessage;
		Reset();
	}

	void Reset()
	{
		Message_lock->lock();
		Message->Size = strlen(defaultMessage);
		strcpy_s(Message->Message, Message->Size + 1, defaultMessage);
		MessageWritten = false;
		Message_lock->unlock();
	}

	void RetrieveMessage(char* message, int& size)
	{
		while (!MessageWritten) {};
		Message_lock->lock();
		size = Message->Size;
		strcpy_s(message, 256, Message->Message);
		Message_lock->unlock();
	}

	/// <summary>
	/// Sets the next action to be performed
	/// </summary>
	/// <param name="action">The action that needs to be performed</param>
	void SetAction(ACTION action)
	{
		Action_lock->lock();
		*this->Action = action;
		Action_lock->unlock();
	}
};