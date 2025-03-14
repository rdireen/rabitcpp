/* ****************************************************************
 * Message Pool
 * DireenTech Inc.  (www.direentech.com)
 * Athr: Harry Direen PhD
 * Date: July 2016
 *
   *******************************************************************/

#include "message_pool.h"
#include "RabitMessage.h"

namespace Rabit
{

    MessagePool::MessagePool(RabitMessage *msg, int poolSize)
    {
        _poolSize = poolSize < 1 ? 1 : poolSize;
        _numberOfFreeMessages = _poolSize;
        _checkedOutArray = new bool[_poolSize];
        _messagePoolArray = new RabitMessage*[_poolSize];

        for (int i = 0; i < _poolSize; i++)
        {
            _checkedOutArray[i] = false;
            //Use Clones of the given message to fill the
            //message pool.
            //_messagePoolArray[i] = std::shared_ptr<RabitMessage>(std::move(msg->Clone()));
            _messagePoolArray[i] = static_cast<RabitMessage *>(msg->Clone().release());
        }
        _nextFreeMessageIdx = 0;
    }

    MessagePool::~MessagePool()
    {
        //Delete all objects and messages created in the constructor
        if(_poolSize > 0 && _messagePoolArray != nullptr)
        {
            for(int i = 0; i < _poolSize; i++)
            {
                delete _messagePoolArray[i];
            }
            delete _messagePoolArray;
            delete _checkedOutArray;
            _poolSize = 0;
            _numberOfFreeMessages = 0;
            _nextFreeMessageIdx = -1;
        }
    }


    int MessagePool::findNextFreeMessage()
    {
        bool found = false;
        _nextFreeMessageIdx = _nextFreeMessageIdx < 0 ? 0 : _nextFreeMessageIdx;
        _nextFreeMessageIdx = _nextFreeMessageIdx >= _poolSize ? 0 : _nextFreeMessageIdx;
        for(int i = 0; i < _poolSize; i++)
        {
            if( _checkedOutArray[_nextFreeMessageIdx] == false)
            {
                found = true;
                break;
            }
            else
            {
                ++_nextFreeMessageIdx;
                _nextFreeMessageIdx = _nextFreeMessageIdx >= _poolSize ? 0 : _nextFreeMessageIdx;
            }
        }
        if( !found)
        {
            _nextFreeMessageIdx = -1;
            _numberOfFreeMessages = 0;
        }
        return _nextFreeMessageIdx;
    }

    int MessagePool::findNumberOfFreeMessages()
    {
        int cntr = 0;
        for(int i = 0; i < _poolSize; i++)
        {
            if (_checkedOutArray[i] == false)
            {
                ++cntr;
            }
        }
        _numberOfFreeMessages = cntr;
        return _numberOfFreeMessages;
    }

    //Check out a message from the pool.  Returns a shared pointer
    //with the message if a message is available.
    //Returns an empty pointer if no message is available.
    RabitMessage*  MessagePool::CheckOutMessage()
    {
        RabitMessage*  msg = nullptr;
        if(_numberOfFreeMessages > 0)
        {
            int idx = findNextFreeMessage();
            if( idx >= 0 )
            {
                msg = _messagePoolArray[idx];
                _checkedOutArray[idx] = true;
                --_numberOfFreeMessages;
                if(_numberOfFreeMessages < 0)
                {
                    //This is an error... reset the number
                    //of free messages.
                    findNumberOfFreeMessages();
                }
            }
        }
        return msg;
    }


    //Check a Message back into the Pool.
    int MessagePool::CheckInMessage(RabitMessage* msg)
    {
        int error = 0;
        bool msgMatchCheckedIn = false;
        if(msg != nullptr)
        {
            msg->Clear();
            for (int i = 0; i < _poolSize; i++)
            {
                if (_messagePoolArray[i] == msg)
                {
                    msgMatchCheckedIn = true;
                    _nextFreeMessageIdx = i;
                    if (!_checkedOutArray[i])
                    {
                        //We are checking in a message that was not checked out,
                        //or the message is being checked in more than once,
                        //both are error conditions.
                        error = 1;
                    } else
                    {
                        _checkedOutArray[i] = false;
                        ++_numberOfFreeMessages;
                        if (_numberOfFreeMessages > _poolSize)
                        {
                            //This is an error...
                             findNumberOfFreeMessages();
                        }
                    }
                    break;
                }
            }
            if (!msgMatchCheckedIn)
            {
                //We are checking in a message that does not belong to this message pool.
                error = 2;
            }
        }
        return error;
    }


}