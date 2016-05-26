#ifndef MANAGER0
#define MANAGER0

#include <string>
#include "Message1.h"
#include "RabitManager.h"

class Manager0 : public Rabit::RabitManager {

private:
  int _number;
  std::shared_ptr<Message1> _message1_sptr;

public:
  Manager0(std::string name ) : RabitManager(name){

    this->SetWakeupTimeDelayMSec(10);
    _message1_sptr = std::make_shared<Message1>("Message1");
    this->AddPublishSubscribeMessage(_message1_sptr->GetMessageTypeName(), _message1_sptr);

  }

  void ExecuteUnitOfWork() final {
    _number++;
    _message1_sptr->a = _number;
    _message1_sptr->PostMessage();
  }

};

#endif //MANAGER0
