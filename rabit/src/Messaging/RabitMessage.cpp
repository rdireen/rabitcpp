#include "RabitMessage.h"
#include "GlobalPublishSubscribeException.h"

namespace Rabit{

  bool RabitMessage::Register_SomethingPublished(const boost::function<void ()> &handler){
    auto gpsm = GetGlobalPublishSubscribeMessageRef();
    if(gpsm != nullptr){
      gpsm->Register_SomethingPublished(handler);
    } else {
      throw GlobalPublishSubscribeException(GetMessageTypeName());
    }
  }

  void RabitMessage::PostMessage() {
    auto gpsm = GetGlobalPublishSubscribeMessageRef();
    if(gpsm != nullptr){
      gpsm->PostMessage(this);
    } else {
      throw GlobalPublishSubscribeException(GetMessageTypeName());
    }
  }

  bool RabitMessage::FetchMessage(){
    bool messChanged = false;
    auto gpsm = GetGlobalPublishSubscribeMessageRef();
    if(gpsm != nullptr){
      messChanged = gpsm->FetchMessage(this);
    } else {
      throw GlobalPublishSubscribeException(GetMessageTypeName());
    }
    return messChanged;
  }
}
