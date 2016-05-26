#ifndef MESSAGE2
#define MESSAGE2

#include <string>
#include "RabitMessage.h"

class Message2 : public Rabit::RabitMessage {

private:
  std::string _msg;

public:
  Message2(std::string name ) : RabitMessage(name){
    _msg = "none";

  }

  virtual std::unique_ptr<Rabit::RabitMessage> Clone() final{
    auto clone = std::unique_ptr<Message2>(new Message2(GetMessageTypeName()));
    clone->CopyBase(this);
    clone->a = this->a;
    return std::move(clone);
  }

  virtual bool CopyMessage(Rabit::RabitMessage* msg) final{
    Rabit::RabitMessage::CopyMessage(msg); // call baseclass
    if(msg->GetTypeIndex() == std::type_index(typeid(MessageA))){
      this->a = static_cast<Message2*>(msg)->a;
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

#endif //MESSAGE2
