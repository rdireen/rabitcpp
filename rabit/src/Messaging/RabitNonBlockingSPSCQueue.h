/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2016
 *
 *******************************************************************/

#ifndef RABIT_DEV_RABITNONBLOCKINGSPSCQUEUE_H
#define RABIT_DEV_RABITNONBLOCKINGSPSCQUEUE_H

#include <string>
#include <memory>
#include <boost/signals2.hpp>
#include <boost/lockfree/spsc_queue.hpp>

namespace Rabit
{

    //The Rabit Message Queue is a wrapper for the Boost lockfree  Single-producer
    //Single-Consumer Queue. (http://www.boost.org/doc/libs/1_60_0/doc/html/lockfree.html)
    //(http://www.boost.org/doc/libs/1_60_0/doc/html/boost/lockfree/spsc_queue.html)
    //The data type T must:
    //  T must have a default constructor
    //  T must be copyable
    //More often than not, a message queue passes a pointer to a message object
    //so the requirements will be met.  The Queue works with sharted_ptr(s).
    //The Rabit Message Queue adds event signals which can be used to indicate
    //that a message has been added or removed from the queue.
    template<class T>
    class RabitNonBlockingSPSCQueue
    {

    private:
        const int UPPER_BOUND = 1000000;        //Absolute max size of the queue.

        std::string _msgQueueName;

    private:
        int _maxNumberOfMessages = 0;

        std::unique_ptr<boost::lockfree::spsc_queue<T>> _messQueue;

        boost::signals2::signal<void()> _sigEnqueue;
        boost::signals2::signal<void()> _sigDequeue;

    public:
        typedef boost::signals2::signal<void()>::slot_type _sigEnqueuSlotType;

        // Accessors
    public:
        std::string GetMessageQueueName()
        {
            return _msgQueueName;
        }

        int GetMaxNoMessagesAllowedInQueue()
        {
            return _maxNumberOfMessages;
        }


        // Methods
    public:
        //Instantiate a new Queue... the maxNoMessages is the maximum
        //number of items the Queue can hold.
        //The msgQName is the the name the queue will be accessed by.
        RabitNonBlockingSPSCQueue(int maxNoMessages, std::string msgQName)
        {
            _msgQueueName = msgQName.empty() ? "MessageQueue" : msgQName;

            maxNoMessages = maxNoMessages < 1 ? 1 : maxNoMessages;
            maxNoMessages = maxNoMessages > UPPER_BOUND ? UPPER_BOUND : maxNoMessages;
            _maxNumberOfMessages = maxNoMessages;

            _messQueue = std::unique_ptr<boost::lockfree::spsc_queue<T>>(new boost::lockfree::spsc_queue<T>(maxNoMessages));
        }


        //Register a Event Handler for when a message is added to the queue.
        void Register_SomethingEnqueued(const boost::function<void()> &handler)
        {
            _sigEnqueue.connect(handler);
        }

        //Register an event handler for when an item is pulled from the queue.
        void Register_SomethingDequeued(const boost::function<void()> &handler)
        {
            _sigDequeue.connect(handler);
        }


        void ClearMessageQueue()
        {
            T item;
             while(_messQueue->pop(item) > 0);
        }


        //Add a message to the Queue.
        //Returns true if the message was added to the queue...
        //returns false if the the queue is full and the message could not be
        //added.
        bool AddMessage(T const &msg)
        {
            bool msgAddedToQueue = _messQueue->push(msg);
            _sigEnqueue();
            return msgAddedToQueue;
        }


        //Add a message to the Queue.
        //Returns true if the message was added to the queue...
        //returns false if the the queue is full and the message could not be
        //added.
        //This method does not trigger a push-message event.
        bool AddMessageNoEventTrigger(T const &msg)
        {
            return _messQueue->push(msg);
        }

        //Get a message from the Message Queue.
        //Returns true if a message is obtained.
        //Returns false if the message queue is empty.
        bool GetMessage(T &msg)
        {
            int noMsgsPopped = _messQueue->pop(msg);
            if(noMsgsPopped > 0)
            {
                _sigDequeue();
            }
            return noMsgsPopped > 0 ? true : false;
        }

    };

}

#endif //RABIT_DEV_RABITNONBLOCKINGSPSCQUEUE_H
