#ifndef _MESSAGEQUEUE_H_
#define _MESSAGEQUEUE_H_

#include "var.h"
#include <deque>

using namespace std;

namespace PewPew
{
    /// a structure to hold message parameters and operation
    typedef struct MessageElem
    {
        string op;
        Var    p1;
        Var    p2;
        Var    p3;

    } MessageElem;

    /// this message queque is used to pass operations that are
    /// called from the lua script.
    class MessageQueue
    {
    protected: 
        typedef deque <MessageElem> DQMessage;
        DQMessage mMessages;
        CRITICAL_SECTION mCS;

    public:
        MessageQueue();
        ~MessageQueue();
        bool getMessage( MessageElem& msgOut );
        bool addMessage( const MessageElem& msgIn );
        void clear();
    };
}

#endif