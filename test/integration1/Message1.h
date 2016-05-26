#ifndef MESSAGE1
#define MESSAGE1

#include <string>
#include "RabitMessage.h"

class Message1 : public Rabit::RabitMessage {

public:
  int a;

public:
  Message1(std::string name ) : RabitMessage(name){
    a = 0;
  }

  virtual std::unique_ptr<Rabit::RabitMessage> Clone() final{
    auto clone = std::unique_ptr<Message1>(new Message1(GetMessageTypeName()));
    clone->CopyBase(this);
    clone->a = this->a;
    return std::move(clone);
  }

  virtual bool CopyMessage(Rabit::RabitMessage* msg) final{
    Rabit::RabitMessage::CopyMessage(msg); // call baseclass
    if(msg->GetTypeIndex() == std::type_index(typeid(Message1))){
      this->a = static_cast<Message1*>(msg)->a;
      return true;
    }
    return false;
  }

  virtual void Clear() final {
    a = 0;
  }

  virtual std::string ToString() const final {
    std::ostringstream os;
    os << "The message is: " << a;
    return os.str();
  }

};

#endif //MESSAGE1
