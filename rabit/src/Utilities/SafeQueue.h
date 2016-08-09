/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2016
 *
 *******************************************************************/

#ifndef SAFEQUEUE
#define SAFEQUEUE

#include <queue>
#include <mutex>
#include <condition_variable>

namespace Rabit
{

    template<class T>
    class SafeQueue
    {
    public:
        SafeQueue(int maxQueueSize)
                : q(), m(), c()
        {
            _queueMaxSize = maxQueueSize;
        }

        SafeQueue(const SafeQueue &other) = delete; // non construction-copyable
        SafeQueue &operator=(const SafeQueue &) = delete; // non copyable

        ~SafeQueue(void)
        {}

        int GetMaxQueueSize()
        {
            return _queueMaxSize;
        }

        //Add an item to the queue... the queue is checked to see if it is
        //full first...
        //Returns true if the item is added to the queue.
        //Return false if the queue is full.
        bool enqueue(T t)
        {
            std::lock_guard<std::mutex> lock(m);
            bool ok = false;
            if(q.size() < _queueMaxSize)
            {
                q.push(t);
                ok = true;
            }
            return ok;
        }

        //Pull and item from the queue.
        //Returns true if item obtained.
        //Returns false if the queue is empty.
        bool dequeue(T &item)
        {
            std::unique_lock<std::mutex> lock(m);
            bool ok = false;
            if (!q.empty())
            {
                item = q.front();
                q.pop();
                ok = true;
            }
            return ok;
        }

        void clear()
        {
            std::lock_guard<std::mutex> lock(m);
            std::queue<T> empty;
            std::swap(q, empty);
        }

        int size()
        {
            std::lock_guard<std::mutex> lock(m);
            return (int)q.size();
        }

        bool empty()
        {
            return q.empty();
        }

    private:
        std::queue<T> q;
        mutable std::mutex m;
        std::condition_variable c;
        int _queueMaxSize = 1000;
    };
}

#endif //SAFEQUEUE
