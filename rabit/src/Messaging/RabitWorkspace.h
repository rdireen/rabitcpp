#ifndef RABIT_WORKSPACE
#define RABIT_WORKSPACE

#include <memory>
#include <unordered_map>
#include <string>
#include <boost/any.hpp>
#include "RabitMessageQueue.h"

namespace Rabit{
  
  class RabitWorkspace{

  public:
    static std::shared_ptr<RabitWorkspace> GetWorkspace(){
      if(rabitWorkspace == nullptr){
        rabitWorkspace = std::make_shared<RabitWorkspace>();
      }
      return rabitWorkspace;
    }

    template<typename T>
    void AddManagerMessageQueue(std::string name, std::shared_ptr<RabitMessageQueue<T>> queue){
      messageQueueDict[name] = queue;
    }

    template<typename T>
    bool AddPublishSubscribeMessage(std::string name, T msg){

    }

    template<typename T>
    bool AddMessageToQueue(std::string name, T msg){
      auto search = messageQueueDict.find(name);
      if(search != messageQueueDict.end()){
        boost::any_cast<std::shared_ptr<RabitMessageQueue<T>>>(messageQueueDict[name])->AddMessage(msg);
        return true;
      }
      return false;
    }

    template<typename T>
    bool Register_DequeuedEvent(std::string name, const boost::function<void ()> &handler){
      auto search = messageQueueDict.find(name);
      if(search != messageQueueDict.end()){
        boost::any_cast<std::shared_ptr<RabitMessageQueue<T>>>(messageQueueDict[name])->Register_SomethingDequeued(handler);
        return true;
      }
      return false;
    }

  private:
    static std::shared_ptr<RabitWorkspace> rabitWorkspace;
    std::unordered_map<std::string, boost::any > messageQueueDict;
  };


}



#endif //RABIT_WORKSPACE
