#include "pch.h"
#include "messagequeue.h"

using namespace PewPew;

//--------------------------------------------------------------------
MessageQueue::MessageQueue()
{
    InitializeCriticalSection(&mCS);
}
//--------------------------------------------------------------------
MessageQueue::~MessageQueue()
{
    mMessages.clear();
    DeleteCriticalSection(&mCS);
}
//--------------------------------------------------------------------
bool MessageQueue::getMessage( MessageElem& msgOut )
{
    EnterCriticalSection(&mCS);
    if (!mMessages.empty())
    {
        MessageElem& msg = mMessages.front();

        msgOut = msg;

        mMessages.pop_front();

        LeaveCriticalSection(&mCS);
        return true;
    }

    LeaveCriticalSection(&mCS);
    return false;
}
//--------------------------------------------------------------------
bool MessageQueue::addMessage( const MessageElem& msgIn )
{
    EnterCriticalSection(&mCS);

    mMessages.push_back(msgIn);

    LeaveCriticalSection(&mCS);
    return true;
}
//--------------------------------------------------------------------
void MessageQueue::clear()
{
    EnterCriticalSection(&mCS);

    mMessages.clear();

    LeaveCriticalSection(&mCS);
}