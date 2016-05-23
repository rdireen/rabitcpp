#ifndef MANAGER_CONTROL_MESSAGE
#define MANAGER_CONTROL_MESSAGE

#include <string>
#include <memory>
#include <sstream>
#include "RabitMessage.h"

namespace Rabit{
  
  class ManagerControlMessage : public RabitMessage{

  private:
    bool _shutdownAllManagers = false;
    bool _runState = false;

  public:
    bool GetShutdownAllManagers(){
      return _shutdownAllManagers;
    }

    void ShutdownAllManagers(bool value){
      _shutdownAllManagers = value;
    }

  public:
    ManagerControlMessage(std::string name) : RabitMessage(name){
      _shutdownAllManagers = false;
      _runState = false;
    }

    virtual std::unique_ptr<RabitMessage> Clone() final{
      auto clone = std::unique_ptr<ManagerControlMessage>(new ManagerControlMessage(GetMessageTypeName()));
      clone->CopyBase(this);
      clone->_shutdownAllManagers = this->_shutdownAllManagers;
      clone->_runState = this->_runState;

      return std::move(clone);
    }

    virtual bool CopyMessage(RabitMessage* msg) final {
      RabitMessage::CopyMessage(msg);
      if(msg->GetTypeIndex() == std::type_index(typeid(ManagerControlMessage))){
        this->_shutdownAllManagers = static_cast<ManagerControlMessage*>(msg)->_shutdownAllManagers;
        this->_runState = static_cast<ManagerControlMessage*>(msg)->_runState;
        return true;
      }
      return false;
    }

    virtual void Clear() final{
      _shutdownAllManagers = false;
      _runState = false;
    }

    virtual std::string ToString() const final {
      std::ostringstream ss;
      ss << "ShutdownAllManagers = " << _shutdownAllManagers
         << "   RunState = " << _runState;
      return ss.str();
    }
  };

}



#endif //MANAGER_CONTROL_MESSAGE
