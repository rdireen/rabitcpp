#ifndef RABIT_WORKSPACE
#define RABIT_WORKSPACE

#include <memory>
#include <unordered_map>
#include <string>
#include <boost/any.hpp>
#include "RabitMessageQueue.h"
#include "PublishSubscribeMessage.h"

namespace Rabit{
  
  class RabitWorkspace{

  private:

    static RabitWorkspace* _rabitWorkspace;

    struct PSMsgContainer{
      std::shared_ptr<RabitMessage> MgrMsgRef;
      std::shared_ptr<PublishSubscribeMessage> PSMsg;
    };

    std::unordered_map<std::string, boost::any > _messageQueueDict;
    std::unordered_map<std::string, std::unique_ptr<PSMsgContainer> > _publishSubscribeMsgDict;

  private:
    RabitWorkspace(){}
    RabitWorkspace(const RabitWorkspace& rws);
    RabitWorkspace& operator = (const RabitWorkspace& rws);

  public:

    static RabitWorkspace* GetWorkspace(){
      if(!_rabitWorkspace)
        _rabitWorkspace = new RabitWorkspace();
      return _rabitWorkspace;
    }

    template<typename T>
    void AddManagerMessageQueue(std::string name, std::shared_ptr<RabitMessageQueue<T>> queue){
      _messageQueueDict[name] = queue;
    }

    template<typename T>
    bool AddMessageToQueue(std::string name, T msg){
      auto search = _messageQueueDict.find(name);
      if(search != _messageQueueDict.end()){
        boost::any_cast<std::shared_ptr<RabitMessageQueue<T>>>(_messageQueueDict[name])->AddMessage(msg);
        return true;
      }
      return false;
    }

    template<typename T>
    bool Register_DequeuedEvent(std::string name, const boost::function<void ()> &handler){
      auto search = _messageQueueDict.find(name);
      if(search != _messageQueueDict.end()){
        boost::any_cast<std::shared_ptr<RabitMessageQueue<T>>>(_messageQueueDict[name])->Register_SomethingDequeued(handler);
        return true;
      }
      return false;
    }

    bool AddPublishSubscribeMessage(std::string name, std::shared_ptr<RabitMessage> msg){
      bool error = false;
      auto search = _publishSubscribeMsgDict.find(name);
      if(search == _publishSubscribeMsgDict.end()){
        auto psMsgContainer = std::unique_ptr<PSMsgContainer>(new PSMsgContainer());
        psMsgContainer->MgrMsgRef = msg;
        psMsgContainer->PSMsg = std::make_shared<PublishSubscribeMessage>(msg->Clone());
        msg->GlobalPublishSubscribeMessageRef(psMsgContainer->PSMsg);
        _publishSubscribeMsgDict[name] = std::move(psMsgContainer);
      } else {
          if(_publishSubscribeMsgDict[name]->PSMsg->MsgTypeIndex() == msg->GetTypeIndex()){
            msg->GlobalPublishSubscribeMessageRef(_publishSubscribeMsgDict[name]->PSMsg);
          }else{
            error = false;
          }
      }
      return error;
    }

    std::unique_ptr<RabitMessage> GetMessage(std::string name){
      auto search = _publishSubscribeMsgDict.find(name);
      std::unique_ptr<RabitMessage>  rm = nullptr;
      if(search == _publishSubscribeMsgDict.end()){
        rm = _publishSubscribeMsgDict[name]->PSMsg->GetCopyOfMessage();
      }
      return std::move(rm);
    }

  };
}



#endif //RABIT_WORKSPACE
