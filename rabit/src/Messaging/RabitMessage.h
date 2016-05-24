#ifndef RABIT_MESSAGE
#define RABIT_MESSAGE

#include <chrono>
#include <string>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include "PublishSubscribeMessage.h"

namespace Rabit{

  class PublishSubscribeMessage;
  class RabitMessage {
  protected:
    typedef std::chrono::high_resolution_clock::time_point TimeStamp;
    TimeStamp _timeStamp;
    std::string _messageTypeName;
    std::shared_ptr<PublishSubscribeMessage> _globalPublishSubscribeMessageRef = nullptr;

  public:
    std::shared_ptr<PublishSubscribeMessage> GetGlobalPublishSubscribeMessageRef(){
      return _globalPublishSubscribeMessageRef;
    }

    void GlobalPublishSubscribeMessageRef(std::shared_ptr<PublishSubscribeMessage> val){
      _globalPublishSubscribeMessageRef = val;
    }

  public:
    RabitMessage(std::string messageTypeName){
      _messageTypeName = messageTypeName;
    }

    virtual std::unique_ptr<RabitMessage> Clone(){}

    virtual bool CopyMessage(RabitMessage* msg){
      _timeStamp = msg->GetTimeStamp();
      _messageTypeName = msg->GetMessageTypeName();
      return true;
    }

    virtual void Clear() = 0;

    virtual std::string ToString() const { }

    void CopyBase(RabitMessage* msg){
      _timeStamp = msg->_timeStamp;
      _messageTypeName = msg->_messageTypeName;
    }

    std::type_index GetTypeIndex(){
      return typeid(*this);
    }

    void SetTimeNow(){
      _timeStamp = std::chrono::high_resolution_clock::now();
    }

    const TimeStamp& GetTimeStamp() const{
      return _timeStamp;
    }

    bool Register_SomethingPublished(const boost::function<void ()> &handler);

    /**
     * @brief PostMessage
     * @throws GlobalPublishSubscribeException
     */
    void PostMessage();

    /**
     * @brief FetchMessage
     * @throws GlobalPublishSubscribeException
     * @return true if the message has been updated
     */
    bool FetchMessage();

    /*
     * If equal in time, this function returns true;
     */
    bool CompareTime(RabitMessage* rabitMessage){
      if(this->GetTimeStamp() == rabitMessage->GetTimeStamp())
        return true;
      else
        return false;
    }

    std::string GetMessageTypeName() const{
      return _messageTypeName;
    }

    virtual ~RabitMessage(){

    }
  };
}



#endif //RABIT_MESSAGE
