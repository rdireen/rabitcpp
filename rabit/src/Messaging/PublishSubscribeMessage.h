#ifndef PUBLISH_SUBSCRIBE_MESSAGE
#define PUBLISH_SUBSCRIBE_MESSAGE

#include <chrono>
#include <memory>
#include "RabitMessage.h"

namespace Rabit{

  class PublishSubscribeMessage {

  private:
    std::shared_ptr<RabitMessage> _statusMessage;

  public:

    PublishSubscribeMessage(std::shared_ptr<RabitMessage> stMessage){
      _statusMessage = stMessage;
    }

    std::shared_ptr<RabitMessage> GetStatusMessage() const{
      return _statusMessage;
    }

    std::string MsgTypeName() const{
      return _statusMessage->GetMessageTypeName();
    }

    typedef std::chrono::high_resolution_clock::time_point TimeStamp;
    const TimeStamp& GetTimeStamp() const{
      return _statusMessage->GetTimeStamp();
    }

    std::shared_ptr<RabitMessage> GetCopyOfMessage(){
      // need to lock this down. Multiple managers have access to it.
      return _statusMessage->Clone();
    }


  };
  
}



#endif //PUBLISH_SUBSCRIBE_MESSAGE
