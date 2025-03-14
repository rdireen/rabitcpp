/* ****************************************************************
 * Message Pool
 * DireenTech Inc.  (www.direentech.com)
 * Athr: Harry Direen PhD
 * Date: July 2016
 *
 *
  *******************************************************************/


#ifndef VIDERE_DEV_MESSAGE_POOL_H
#define VIDERE_DEV_MESSAGE_POOL_H


#include <iostream>
#include <string>
#include <memory>
#include "RabitMessage.h"

namespace Rabit
{
    //A Message Pool is a fixed size pool of the same type of messages.
    //A message can be checked out... used over time and then checked back in.
    //This is ideal for messages that hold large amounts of data such as images
    //and the user would like to avoid constantly allocating and then dealocating
    //the memory for the messges.
    class MessagePool
    {
    private:
        int _poolSize = 0;   //The Total number of messages in the pool

        //An array of flags indicating whether or not a message has been checked out.
        bool *_checkedOutArray;

        //An array of messages that can be checked out.
        //RabitMessage **_messagePoolArray;
        RabitMessage  **_messagePoolArray;

        //An index to the next available message.
        //If less than zero... the all messages have been checked out.
        int _nextFreeMessageIdx = -1;

        //Number of available messages.
        int _numberOfFreeMessages = 0;

    public:
        //Constructor... pass in a message of the type to be
        //used in the message pool and the poolSize (total
        //number of messages to be in the pool)
        MessagePool(RabitMessage *msg, int poolSize);

        //Becareful with the destructor... there is no check
        //to verify that all messages have been checked back in.
        //Checked out Mesages that are being used will be deleted!
        ~MessagePool();

        int GetNumberOfFreeMessages()
        {
            return _numberOfFreeMessages;
        }

        int GetNumberOfCheckedOutMessages()
        {
            return _poolSize - _numberOfFreeMessages;
        }

        int GetPoolSize()
        {
            return _poolSize;
        }

        //Check out a message from the pool.  Returns a shared pointer
        //with the message if a message is available.
        //Returns an empty pointer if no message is available.
        RabitMessage* CheckOutMessage();

        //Check a Message back into the Pool.
        //Returns zero if message checked in ok...
        //returns non-zero if there was an error checking the message in:
        //   1)  message already checked in
        //   2)  There was no matching message in the pool
        int CheckInMessage(RabitMessage* msg);

        //A helper function to walk the list of messages and
        //count the number of free messages;
        //Resets the _numberOfFreeMessages value.
        //Can be used to verify GetNumberOfFreeMessages... otherwise it
        //should not be used for normal operation as it causes a search of
        //the message pool list.
        int findNumberOfFreeMessages();


    private:
        //A helper function to set the _nextFreeMessageIdx
        //to the next free message.  Returns -1 if no free messages
        //are available.
        int findNextFreeMessage();

    };

}
#endif //VIDERE_DEV_MESSAGE_POOL_H
