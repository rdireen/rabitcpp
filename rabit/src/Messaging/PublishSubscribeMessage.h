#ifndef PUBLISH_SUBSCRIBE_MESSAGE
#define PUBLISH_SUBSCRIBE_MESSAGE

#include <chrono>
#include <memory>
#include <mutex>
#include <typeinfo>
#include <typeindex>
#include "RabitMessage.h"

namespace Rabit{

  class RabitMessage;
  class PublishSubscribeMessage {

  private:
    std::unique_ptr<RabitMessage> _statusMessage;
    mutable std::mutex _mutex;

  public:

    PublishSubscribeMessage(std::unique_ptr<RabitMessage> stMessage){
      _statusMessage = std::move(stMessage);
    }
  /*
    std::shared_ptr<RabitMessage> GetStatusMessage() const{
      std::lock_guard<std::mutex> lk(_mutex);
      return _statusMessage;
    }
    */

    std::type_index MsgTypeIndex() const;

    std::unique_ptr<RabitMessage> GetCopyOfMessage();

    void PostMessage(RabitMessage* msg_ptr);

    bool FetchMessage(RabitMessage* msg_ptr);

    void ForceFetchMessage(RabitMessage* msg_ptr);

    /**
     * @brief GetTestMessage just used for testing
     * @return
     */
    const std::unique_ptr<RabitMessage>& GetTestMessage(){
      return _statusMessage;
    }

  };
}

#endif //PUBLISH_SUBSCRIBE_MESSAGE
