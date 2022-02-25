#include "WorkerThread.h"

/// <summary>
/// Constructs the worker thread
/// </summary>
/// <param name="p_defaultMessage">The default message</param>
WorkerThread::WorkerThread(const char* p_defaultMessage)
{
	this->defaultMessage = p_defaultMessage;
	this->Message = new char[strlen(p_defaultMessage)];
	Reset();
}

/// <summary>
/// Resets the worker thread
/// </summary>
void WorkerThread::Reset()
{
	*MessageSize = strlen(defaultMessage);
	strcpy_s(Message, *MessageSize + 1, defaultMessage);
	MessageWritten = false;
}

/// <summary>
/// Retrieves the message that was received by the thread
/// </summary>
/// <param name="p_message">The message buffer</param>
/// <param name="p_size">The size of the buffer</param>
void WorkerThread::RetrieveMessage(char* p_messageBuffer, int& p_size)
{
	while (!MessageWritten) {};
	strcpy_s(p_messageBuffer, p_size, this->Message);
	p_size = *this->MessageSize;
}

/// <summary>
/// Sets the next action to be performed
/// </summary>
/// <param name="p_action">The action that needs to be performed</param>
void WorkerThread::SetAction(ACTION p_action)
{
	Action_lock->lock();
	*this->Action = p_action;
	Action_lock->unlock();
}

/// <summary>
/// Gets the action from the main thread
/// </summary>
/// <returns>The action that needs to be executed</returns>
int WorkerThread::GetAction()
{
	Action_lock->lock();
	ACTION action = *Action;
	if (action != ACTION_NONE)
	{
		*Action = ACTION_NONE;
	}
	Action_lock->unlock();
	return action;
}

/// <summary>
/// Loops the worker thread until stopped
/// </summary>
void WorkerThread::Loop()
{
	ACTION action = ACTION_NONE;
	while ((action = GetAction()) != ACTION_STOP)
	{
		PerformAction(action);
	}
}

