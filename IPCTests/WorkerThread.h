#pragma once
#include <thread>
#include <mutex>
class WorkerThread
{
protected:
	std::mutex* Action_lock = new std::mutex();
	ACTION* Action = new ACTION(-1);
	bool MessageWritten;
	const char* defaultMessage;
	char* Message = nullptr;
	int* MessageSize = new int(-1);
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
		this->Message = new char[strlen(defaultMessage)];
		Reset();
	}
	

	void Reset()
	{
		*MessageSize = strlen(defaultMessage);
		strcpy_s(Message, *MessageSize + 1, defaultMessage);
		MessageWritten = false;
	}

	void RetrieveMessage(char* message, int& size)
	{
		while (!MessageWritten) {};
		strcpy_s(message, size, this->Message);
		size = *this->MessageSize;
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