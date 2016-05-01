#ifndef RABIT_MESSAGE_QUEUE
#define RABIT_MESSAGE_QUEUE

#include <string>
#include <memory>
#include "SafeQueue.h"

namespace Rabit{

  template<class T>
  class RabitMessageQueue{

  private:
    const int UPPER_BOUND = 1000000;

    std::string _msgQueueName;
    int _maxNoMessagesAllowedInQueue;
    std::unique_ptr<SafeQueue<T>> _messQueue;

  // Accessors
  public:
    std::string GetMessageQueueName(){
      return _msgQueueName;
    }

    int GetMaxNoMessagesAllowedInQueue(){
      return _maxNoMessagesAllowedInQueue;
    }

    void SetMaxNoMessagesAllowedInQueue(int value){
      value = value < 1 ? 1 : value;
      _maxNoMessagesAllowedInQueue = value > UPPER_BOUND ? UPPER_BOUND : value;
    }

  // Methods
  public:
    RabitMessageQueue(int maxNoMessages, std::string msgQName){
      SetMaxNoMessagesAllowedInQueue(maxNoMessages);
      _msgQueueName = msgQName.empty() ? "MessageQueue" : msgQName;
      _messQueue = std::unique_ptr<SafeQueue<T>>(new SafeQueue<T>());
    }

    void ClearMessageQueue(){
      _messQueue->clear();
    }

    int NoMessagesInQueue(){
      return _messQueue->size();
    }

    bool AddMessage(T msg){
      if( _messQueue->size() < _maxNoMessagesAllowedInQueue)
      {
          _messQueue->enqueue(msg);
          // Fire off trigger
          return true;
      }else{
          return false;
      }
    }

    bool AddMessageNoEventTrigger(T msg){
      if( _messQueue->size() < _maxNoMessagesAllowedInQueue)
      {
          _messQueue->enqueue(msg);
          return true;
      }else{
          return false;
      }
    }

    T GetMessage(){ //This blocks if nothing is in it. Put a wait in it?
      T msg;
      if( _messQueue->size() > 0){
        msg = _messQueue->dequeue();
      }
      return msg;
    }

  };
  
}



#endif //RABIT_MESSAGE_QUEUE
