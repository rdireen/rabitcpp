
#include "PublishSubscribeMessage.h"

namespace Rabit{

  std::type_index PublishSubscribeMessage::MsgTypeIndex() const{
    std::lock_guard<std::mutex> lk(_mutex);
    return _statusMessage->GetTypeIndex();
  }

  std::unique_ptr<RabitMessage> PublishSubscribeMessage::GetCopyOfMessage(){
    std::lock_guard<std::mutex> lk(_mutex);
    return std::move(_statusMessage->Clone());
  }

  void PublishSubscribeMessage::PostMessage(RabitMessage* msg_ptr){
    std::lock_guard<std::mutex> lk(_mutex);
    msg_ptr->SetTimeNow();
    _statusMessage->CopyMessage(msg_ptr);
  }

  bool PublishSubscribeMessage::FetchMessage(RabitMessage* msg_ptr){
    std::lock_guard<std::mutex> lk(_mutex);
    if(!_statusMessage->CompareTime(msg_ptr)){
      msg_ptr->CopyMessage(_statusMessage.get());
      return true;
    }
    return false;
  }

  void PublishSubscribeMessage::ForceFetchMessage(RabitMessage* msg_ptr){
    std::lock_guard<std::mutex> lk(_mutex);
    msg_ptr->CopyMessage(_statusMessage.get());
  }

}
