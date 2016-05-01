
#ifndef SAFEQUEUE
#define SAFEQUEUE

#include <queue>
#include <mutex>
#include <condition_variable>

namespace Rabit{

  template <class T>
  class SafeQueue
  {
  public:
    SafeQueue(void)
      : q()
      , m()
      , c()
    {}

    SafeQueue( const SafeQueue& other ) = delete; // non construction-copyable
    SafeQueue& operator=( const SafeQueue& ) = delete; // non copyable

    ~SafeQueue(void)
    {}

    void enqueue(T t)
    {
      std::lock_guard<std::mutex> lock(m);
      q.push(t);
      c.notify_one();
    }

    T dequeue(void)
    {
      std::unique_lock<std::mutex> lock(m);
      while(q.empty())
      {
        c.wait(lock);
      }
      T val = q.front();
      q.pop();
      return val;
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
      return q.size();
    }

    bool empty()
    {
      return q.empty();
    }

  private:
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable c;
  };
}

#endif //SAFEQUEUE
