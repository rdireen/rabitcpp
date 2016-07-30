#ifndef RABIT_MESSAGE_QUEUE
#define RABIT_MESSAGE_QUEUE

#include <string>
#include <memory>
#include <boost/signals2.hpp>
#include "SafeQueue.h"

namespace Rabit
{

    template<class T>
    class RabitMessageQueue
    {

    private:
        const int UPPER_BOUND = 1000000;

        std::string _msgQueueName;
        int _maxNoMessagesAllowedInQueue;
        std::unique_ptr<SafeQueue<T>> _messQueue;
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
            return _maxNoMessagesAllowedInQueue;
        }

        void SetMaxNoMessagesAllowedInQueue(int value)
        {
            value = value < 1 ? 1 : value;
            _maxNoMessagesAllowedInQueue = value > UPPER_BOUND ? UPPER_BOUND : value;
        }

        // Methods
    public:
        RabitMessageQueue(int maxNoMessages, std::string msgQName)
        {
            SetMaxNoMessagesAllowedInQueue(maxNoMessages);
            _msgQueueName = msgQName.empty() ? "MessageQueue" : msgQName;
            _messQueue = std::unique_ptr<SafeQueue<T>>(new SafeQueue<T>());
        }

        void ClearMessageQueue()
        {
            _messQueue->clear();
        }

        int NoMessagesInQueue()
        {
            return _messQueue->size();
        }

        bool AddMessage(T msg)
        {
            if (_messQueue->size() < _maxNoMessagesAllowedInQueue)
            {
                _messQueue->enqueue(msg);
                _sigEnqueue();
                return true;
            } else
            {
                return false;
            }
        }

        void Register_SomethingEnqueued(const boost::function<void()> &handler)
        {
            _sigEnqueue.connect(handler);
        }

        void Register_SomethingDequeued(const boost::function<void()> &handler)
        {
            _sigDequeue.connect(handler);
        }

        bool AddMessageNoEventTrigger(T msg)
        {
            if (_messQueue->size() < _maxNoMessagesAllowedInQueue)
            {
                _messQueue->enqueue(msg);
                return true;
            } else
            {
                return false;
            }
        }

        T GetMessage()
        {
            T msg;
            if (!_messQueue->empty())
            {
                msg = _messQueue->dequeue();
                _sigDequeue();
            }
            return msg;
        }

    };

}


#endif //RABIT_MESSAGE_QUEUE
